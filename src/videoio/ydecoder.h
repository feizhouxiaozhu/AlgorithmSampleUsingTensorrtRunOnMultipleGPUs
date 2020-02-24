






#ifndef YDECODER_H
#define YDECODER_H

extern "C" {
#include "libavcodec/avcodec.h"
}

#include <QList>
#include <QTimer>
#include <QThread>
#include <QObject>

#include "ysharedframe.h"
#include "utils/ythreadworker.h"

class YDecoderWorker : public YThreadWorker
{
    Q_OBJECT
public:
    explicit YDecoderWorker(int stream_id);
    virtual ~YDecoderWorker();

signals:
    void signal_frameReady(int id, YSharedFrame* frame);

public slots:
    void slot_setupFrameBuffer(const QList<YSharedFrame*>& frame_list);
    virtual void slot_setupDecoder(AVCodecParameters *ctx) = 0;
    virtual void slot_packetReady(YSharedPacket *pkt) = 0;

protected:
    void create() override;
    void release() override;

protected slots:
    void slot_outputReadyFrame();

protected:
    YSharedFrame* getFreeFrame();

protected:
    int m_stream_id;
    QTimer *m_timer;
    QList<YSharedFrame*> ml_free_frame;
    QList<YSharedFrame*> ml_ready_frame;
};

class NvDevice;

class YDecoder : public QThread
{
    Q_OBJECT
public:
    YDecoder(int id, QObject *parent = nullptr);
    ~YDecoder();

    typedef enum enYDecoderType {
        YDecoderType_Ffmpeg,
        YDecoderType_Nvcuvid
    } YDecoderType;

    void createWorker(YDecoderType decoder_type);

    /* set up format of output frame, should be called before setupFrameBuffer.
     * If not set up, the default value is 1280,720,YPixFormat_BGR */
    void setupOutputFormat(int w, int h, YPixelFormat fmt);

    /* create buffer used for decoder worker */
    static const int DEFAULT_BUFFER_COUNT = 32;
    void createFrameBuffer(YDataType data_type, int buffer_count = DEFAULT_BUFFER_COUNT);

    void startDecoder();
    void stopDecoder();

    void releaseFrameBuffer();

signals:
    void signal_setupFrameBuffer(const QList<YSharedFrame*>& frame_list);
    void signal_setupDecoder(AVCodecParameters *ctx);
    void signal_packetReady(YSharedPacket *pkt);
    void signal_frameReady(int id, YSharedFrame* frame);

protected:
    int      m_id;
    int      m_width;
    int      m_height;
    YPixelFormat m_format;

    NvDevice*            m_device;
    QList<YSharedFrame*> ml_frame;
    YDecoderWorker      *m_worker;
};

#endif // YDECODER_H
