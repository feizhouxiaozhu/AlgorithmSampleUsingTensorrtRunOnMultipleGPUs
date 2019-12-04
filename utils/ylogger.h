






#ifndef YLOGGER_H
#define YLOGGER_H

#include <iostream>

typedef enum enYLoggerModule {
    Module_Utils      = 0x00000001,
    Module_Max
} YLoggerModule;

typedef enum enYLoggerSeverity {
    LoggerSeverity_INFO      = 0x00000000,
    LoggerSeverity_WARNING   = 0x00000000,
    LoggerSeverity_ERROR     = 0x00000000,
    LoggerSeverity_FATAL     = 0x00000000,
    LoggerSeverity_NONE
} YLoggerSeverity;

void initYLogger(const char* program, const char* log_path);
void enableYLoggerModule(int module);
void disableYLoggerModule(int module);
void setYLoggerSeverity(int severity);

#define YLOG(module, severity) YLogger(__FILE__, __LINE__, module, LoggerSeverity_##severity).log()

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

#endif // YLOGGER_H
