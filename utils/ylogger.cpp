




#include "ylogger.h"

/* severity is INFO, and no module is disabled by default */
int g_severity = google::GLOG_INFO;
int g_modules_disabled = 0;

/* TODO: set log file size and so on */
void initYLogger(const char* program, const char* log_path)
{
    google::InitGoogleLogging(program);
}

void enableYLoggerModule(int module)
{
    g_modules_disabled &= ~module;
}

void disableYLoggerModule(int module)
{
    g_modules_disabled |= module;
}

void setYLoggerSeverity(int severity)
{
    g_severity = severity;
}

std::ostream& YLogger::log()
{
    if ((m_module & g_modules_disabled) || m_severity < g_severity)
    {
        return m_dummy;
    }

    m_logger = new google::LogMessage(m_file, m_line, m_severity);
    return m_logger->stream();
}
