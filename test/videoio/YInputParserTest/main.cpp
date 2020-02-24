





#include <QApplication>

#include "videoio/yinputparser.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString url("rtmp://10.16.0.101:1935/echolive/yyl");
    YInputParser *parser = new YInputParser(url, 1030);
    parser->startParser();

    return a.exec();
}
