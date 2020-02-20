





#include <glog/logging.h>
#include <glog/log_severity.h>

#include <QDir>
#include <QFile>
#include <QString>

#include "ylogger.h"

/* severity is INFO, and no module is disabled by default */
int g_severity = Severity_INFO;
int g_modules_disabled = 0;

YLoggerManager* YLoggerManager::m_instance = new YLoggerManager();

YLogger::YLogger(const char* file, int line, int module, int severity) : m_dummy(nullptr)
{
    m_file   = file;
    m_line   = line;
    m_module = module;
    m_severity = severity;
    m_logger = nullptr;
}

YLogger::~YLogger() {
    if (nullptr != m_logger) {
        delete m_logger;
    }
}

std::ostream& YLogger::log()
{
    if ((m_module & g_modules_disabled) || m_severity < g_severity)
    {
        return m_dummy;
    }

    int google_severity = m_severity + google::GLOG_INFO - Severity_INFO;
    google_severity = (google_severity >= google::GLOG_INFO ? google_severity : google::GLOG_INFO);
    m_logger = new google::LogMessage(m_file, m_line, google_severity);
    return m_logger->stream();
}

void YLoggerManagerWorker::slot_setMaxLogFileCount(const char *path, int max_log_file_count) {
    m_path = path;
    m_max_log_file_count = max_log_file_count;
}

void YLoggerManagerWorker::slot_clearLogFile() {
    QDir dir(m_path);
    if (m_path.isEmpty() || !dir.exists()) {
        return;
    }

    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Time);
    QStringList log_files = dir.entryList();
    int count = log_files.size() - m_max_log_file_count;
    while (count-- > 0) {
        QString filename = log_files.takeLast();
        filename.prepend(dir.absolutePath().append("/"));
        QFile log_file(filename);
        log_file.remove();
    }
}

void YLoggerManagerWorker::create() {
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slot_clearLogFile()), Qt::QueuedConnection);
    m_timer->start(10 * 1000);
}

void YLoggerManagerWorker::release() {
    if (nullptr != m_timer) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }
}

YLoggerManager::YLoggerManager() : QObject(nullptr)
{
    google::InitGoogleLogging("Y");
    google::SetStderrLogging(google::GLOG_INFO);

    m_worker = new YLoggerManagerWorker();
    connect(this, SIGNAL(signal_setMaxLogFileCount(const char*,int)),
            m_worker, SLOT(slot_setMaxLogFileCount(const char*,int)), Qt::QueuedConnection);

    m_thread = new QThread(this);
    m_thread->setObjectName("YLogger");
    m_worker->moveToThread(m_thread);
    m_thread->start();
    m_worker->createAsync();
}

YLoggerManager::~YLoggerManager()
{
    m_worker->releaseAsync();
    m_thread->quit();
    m_thread->wait();

    delete m_worker;
    delete m_thread;

    google::ShutdownGoogleLogging();
}

void YLoggerManager::setYLoggerPath(const char *path, int max_log_file_count, int max_size_per_log_file)
{
    QDir dir(path);
    if (dir.exists() && max_log_file_count > 0 && max_size_per_log_file > 0)
    {
        fLS::FLAGS_log_dir = path;
        fLI::FLAGS_logbufsecs = 0;
        fLI::FLAGS_max_log_size = max_size_per_log_file;
        emit signal_setMaxLogFileCount(path, max_log_file_count);
    }
}

void YLoggerManager::enableYLoggerModule(YLoggerModule module)
{
    g_modules_disabled &= ~module;
}

void YLoggerManager::disableYLoggerModule(YLoggerModule module)
{
    g_modules_disabled |= module;
}

void YLoggerManager::setYLoggerSeverity(YLoggerSeverity severity)
{
    g_severity = severity;
}
