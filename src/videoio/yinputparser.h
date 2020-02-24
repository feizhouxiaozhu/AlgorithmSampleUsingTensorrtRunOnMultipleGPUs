





#ifndef YINPUTPARSER_H
#define YINPUTPARSER_H

#include <QThread>

#include "ydecoder.h"

class YInputParser : public QThread
{
    Q_OBJECT
public:
    YInputParser(const QString& url, int stream_id, QObject *parent = nullptr);
    ~YInputParser();

    void startParser();
    void stopParser();

signals:
    void signal_frameReady(int id, YSharedFrame* frame);
    void signal_parseError(int id, QString error);
    void signal_parseDone(int id);

signals:
    void signal_setupDecoder(AVCodecParameters *ctx);
    void signal_packetReady(YSharedPacket *pkt);

protected:
    volatile bool m_stop;
    void run();

private:
    YDecoder* createDecoder(AVCodecParameters *codec_par);
    void releaseDecoder(YDecoder* decoder);
    YSharedPacket* getFreePacket();

private:
    int     m_stream_id;
    QString m_url;
    QList<YSharedPacket*> ml_packet;
};

#endif // YINPUTPARSER_H
