






#ifndef YLOGGER_H
#define YLOGGER_H

#include <iostream>

#include <QTimer>
#include <QThread>

#include "ythreadworker.h"

typedef enum enYLoggerModule {
    Module_Test       = 0x00000001,
    Module_Utils      = 0x00000002,
    Module_Videoio    = 0x00000004
} YLoggerModule;

typedef enum enYLoggerSeverity {
    Severity_DEBUG     = 0x00000000,
    Severity_INFO      = 0x00000001,
    Severity_WARNING   = 0x00000002,
    Severity_ERROR     = 0x00000003,
    Severity_FATAL     = 0x00000004,
    Severity_NONE
} YLoggerSeverity;

#define YLOG(module, severity) YLogger(__FILE__, __LINE__, module, severity).log()

namespace google {
    class LogMessage;
}

class YLogger
{
public:
    YLogger(const char* file, int line, int module, int severity);
    ~YLogger();

    std::ostream& log();

private:
    const char* m_file;
    int m_line;
    int m_module;
    int m_severity;
    std::ostream m_dummy;
    google::LogMessage *m_logger;
};

class YLoggerManagerWorker : public YThreadWorker
{
    Q_OBJECT
public:
    explicit YLoggerManagerWorker() {
        m_timer = nullptr;
    }
    ~YLoggerManagerWorker() {}

public slots:
    void slot_setMaxLogFileCount(const char *path, int max_log_file_count);

protected slots:
    void slot_clearLogFile();

protected:
    void create() override;
    void release() override;

private:
    QTimer* m_timer;
    QString m_path;
    int m_max_log_file_count;
};

class YLoggerManager : public QObject
{
    Q_OBJECT
public:
    static YLoggerManager* getInstance() {
        return m_instance;
    }

    ~YLoggerManager();

    void setYLoggerPath(const char* path, int max_log_file_count = 30, int max_size_per_log_file = 10);
    void enableYLoggerModule(YLoggerModule module);
    void disableYLoggerModule(YLoggerModule module);
    void setYLoggerSeverity(YLoggerSeverity severity);

signals:
    void signal_setMaxLogFileCount(const char *path, int max_log_file_count);

private:
    YLoggerManager();
    YLoggerManager(const YLoggerManager&) : QObject(nullptr) {}
    YLoggerManager& operator =(const YLoggerManager&) = delete;

private:
    QThread*               m_thread;
    YLoggerManagerWorker*  m_worker;
    static YLoggerManager* m_instance;
};

#endif // YLOGGER_H
