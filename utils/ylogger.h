






#ifndef YLOGGER_H
#define YLOGGER_H

#include <glog/logging.h>
#include <glog/log_severity.h>

#include <iostream>

/* The logger adds module control along with sensity level on glog.
 *
 */

typedef enum enLoggerModule {
    Module_Utils      = 0x00010000,
    Module_Max
} LoggerModule;

void initYLogger(const char* program, const char* log_path);
void enableYLoggerModule(int module);
void disableYLoggerModule(int module);
void setYLoggerSeverity(int severity);

#define YLOG(module, severity) YLogger(__FILE__, __LINE__, module, google::severity).log()

class YLogger
{
public:
    YLogger(const char* file, int line, int module, int severity) : m_dummy(nullptr)
    {
        m_file   = file;
        m_line   = line;
        m_module = module;
        m_severity = severity;
        m_logger = nullptr;
    }
    ~YLogger() {
        if (nullptr != m_logger) {
            delete m_logger;
        }
    }

    std::ostream& log();

private:
    const char* m_file;
    int m_line;
    int m_module;
    int m_severity;
    std::ostream m_dummy;

    google::LogMessage *m_logger;
};

#endif // YLOGGER_H
