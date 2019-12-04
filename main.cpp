




//#include <QObject>
#include <QApplication>

#include "utils/ylogger.h"

#include "test/qtimertest.h"

int main(int argc, char** argv)
{
    initYLogger(argv[0], "/home/yyl/Desktop/log");

    QApplication a(argc, argv);

    return a.exec();
}
