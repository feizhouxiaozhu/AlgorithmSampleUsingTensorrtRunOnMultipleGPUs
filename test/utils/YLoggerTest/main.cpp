





#include <QCoreApplication>

#include "../../../src/utils/ylogger.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    YLoggerManager::getInstance()->setYLoggerPath("/home/yyl/Desktop/log", 30, 2);
    YLoggerManager::getInstance()->setYLoggerSeverity(Severity_DEBUG);

    YLOG(Module_Test, Severity_DEBUG) << "Test debug log!";
    YLOG(Module_Test, Severity_INFO) << "Test info log!";
    YLOG(Module_Test, Severity_WARNING) << "Test warning log!";
    YLOG(Module_Test, Severity_ERROR) << "Test error log!";

    return a.exec();
}
