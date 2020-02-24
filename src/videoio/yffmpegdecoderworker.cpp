





extern "C" {
#include "libavutil/imgutils.h"
#include "libavcodec/avcodec.h"
}

#include "utils/ylogger.h"
#include "utils/nvdevicemanager.h"

#include "yffmpegdecoderworker.h"


YFfmpegDecoderWorker::YFfmpegDecoderWorker(int stream_id)
    : YDecoderWorker(stream_id)
{
    m_ctx    = nullptr;
    m_swsctx = nullptr;

    m_pkt          = nullptr;
    m_frame_origin = nullptr;
    m_frame_scaled = nullptr;
}

YFfmpegDecoderWorker::~YFfmpegDecoderWorker()
{
    av_packet_free(&m_pkt);
    av_frame_free(&m_frame_origin);
    av_frame_free(&m_frame_scaled);

    avcodec_free_context(&m_ctx);
    sws_freeContext(m_swsctx);
}

AVPixelFormat YFfmpegDecoderWorker::getAvPixelFormat(YPixelFormat yformat)
{
    AVPixelFormat format = AV_PIX_FMT_NONE;
    switch (format) {
        case YPixelFormat_BGR:
            format = AV_PIX_FMT_BGR24;
            break;
        case YPixelFormat_RGB:
            format = AV_PIX_FMT_RGB24;
            break;
        case YPixelFormat_BGRA:
            format = AV_PIX_FMT_BGRA;
            break;
        case YPixelFormat_RGBA:
            format = AV_PIX_FMT_RGBA;
            break;
        case YPixelFormat_YUV420P:
            format = AV_PIX_FMT_YUV420P;
            break;
        case YPixelFormat_YUV422P:
            format = AV_PIX_FMT_YUV422P;
            break;
        case YPixelFormat_YUV444P:
            format = AV_PIX_FMT_YUV444P;
            break;
    default:
        YLOG(Module_Videoio, Severity_WARNING) << "Unknown pixel format-->" << yformat << "!";
    }

    return format;
}

void YFfmpegDecoderWorker::slot_setupDecoder(AVCodecParameters *ctx)
{
    AVCodec *codec = avcodec_find_decoder(ctx->codec_id);
    if (nullptr == codec) {
        return;
    }

    m_ctx = avcodec_alloc_context3(codec);
    if (nullptr == m_ctx) {
        return;
    }

    /* set essential infos for decoder context */
    m_ctx->thread_count = 1;
    if (codec->capabilities & AV_CODEC_CAP_TRUNCATED) {
        m_ctx->flags |= AV_CODEC_FLAG_TRUNCATED;
    }
    if (0 != ctx->extradata_size) {
        m_ctx->extradata = (uint8_t*)malloc(ctx->extradata_size);
        memcpy(m_ctx->extradata, ctx->extradata, ctx->extradata_size);
        m_ctx->extradata_size = ctx->extradata_size;
    }

    if (avcodec_open2(m_ctx, codec, nullptr) < 0) {
        avcodec_free_context(&m_ctx);
        m_ctx = nullptr;
        return ;
    }

    m_pkt = av_packet_alloc();
    m_frame_origin = av_frame_alloc();

#if 0
    /* restart timer if frame rate is present */
    if (0 != ctx->framerate.num) {
        int interlace = ctx->framerate.den * 1000 / ctx->framerate.num;
        m_timer->start(interlace);
    }
#endif

    YLOG(Module_Videoio, Severity_INFO) << "Video decoder has been created-->" << m_stream_id;
}

void YFfmpegDecoderWorker::slot_packetReady(YSharedPacket *ypkt)
{
    if (nullptr == ypkt) {
        /* TODO: flush decoder */
        return;
    }

    if (nullptr == m_ctx)
    {
        YLOG(Module_Videoio, Severity_ERROR) << "Decoder content hasnot been built!";
        return;
    }

    int ret = 0;
//    int got_picture = 0;
    m_pkt->size = ypkt->getSize();
    m_pkt->data = (uint8_t*)ypkt->getData();

    ret = avcodec_send_packet(m_ctx, m_pkt);
    ret = avcodec_receive_frame(m_ctx, m_frame_origin);
//    ret = avcodec_decode_video2(m_ctx, m_frame_origin, &got_picture, m_pkt);

    ypkt->unref();
    m_pkt->size = 0;
    m_pkt->data = nullptr;
    if (/*got_picture && */(ret >= 0)) {

        YSharedFrame* oframe = getFreeFrame();
        if (nullptr == oframe) {
            YLOG(Module_Videoio, Severity_WARNING) << "Failed to find free frame, drop this ready frame instead!";
            return;
        }

        if (nullptr == m_frame_scaled) {
            m_frame_scaled = av_frame_alloc();
            m_frame_scaled->width  = oframe->getPixelWidth();
            m_frame_scaled->height = oframe->getPixelHeight();
            m_frame_scaled->format = getAvPixelFormat(oframe->getPixelFormat());
            av_image_alloc(m_frame_origin->data, m_frame_origin->linesize,
                           oframe->getPixelWidth(), oframe->getPixelHeight(), (AVPixelFormat)m_frame_scaled->format, sizeof(void*));
        }

        if (nullptr == m_swsctx) {
            m_swsctx = sws_getContext(m_frame_origin->width, m_frame_origin->height, (AVPixelFormat)m_frame_origin->format,
                                      oframe->getPixelWidth(), oframe->getPixelHeight(), (AVPixelFormat)m_frame_scaled->format,
                                      SWS_BICUBIC, nullptr, nullptr, nullptr);
        }
        sws_scale(m_swsctx, m_frame_origin->data, m_frame_origin->linesize, 0, m_frame_origin->height, m_frame_scaled->data, m_frame_scaled->linesize);

        NvDevice* nv_device = nullptr;
        if (YDataType_Device == oframe->getDataType()) {
            nv_device = NvDeviceManager::getInstance()->requireDeviceByDeviceId(oframe->getDeviceId());
            nv_device->pushCtx();
        }

        oframe->ref();
        oframe->copyFromHost(m_frame_scaled->data[0], m_frame_scaled->linesize[0]);
        emit signal_frameReady(m_stream_id, oframe);

        if (YDataType_Device == oframe->getDataType()) {
            nv_device->popCtx();
        }
    }
}
