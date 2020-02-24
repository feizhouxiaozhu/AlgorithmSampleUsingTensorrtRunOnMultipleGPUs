





#ifndef YFFMPEGDECODER_H
#define YFFMPEGDECODER_H

extern "C" {
#include "libswscale/swscale.h"
}

#include "ydecoder.h"

class YFfmpegDecoderWorker : public YDecoderWorker
{
    Q_OBJECT
public:
    YFfmpegDecoderWorker(int stream_id);
    ~YFfmpegDecoderWorker();

public slots:
    void slot_setupDecoder(AVCodecParameters *ctx) override;
    void slot_packetReady(YSharedPacket *pkt) override;

private:
    AVPixelFormat getAvPixelFormat(YPixelFormat yformat);

private:
    AVCodecContext *m_ctx;
    SwsContext     *m_swsctx;

    AVPacket       *m_pkt;
    AVFrame        *m_frame_origin;
    AVFrame        *m_frame_scaled;
};

#endif // YFFMPEGDECODER_H
