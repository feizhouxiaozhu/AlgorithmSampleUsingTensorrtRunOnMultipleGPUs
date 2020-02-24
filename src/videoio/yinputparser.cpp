





extern "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

#include "yinputparser.h"
#include "utils/ylogger.h"

YInputParser::YInputParser(const QString& url, int stream_id, QObject *parent) : QThread(parent)
{
    m_url       = url;
    m_stream_id = stream_id;
}

YInputParser::~YInputParser()
{

}

void YInputParser::startParser()
{
    if (!isRunning()) {
        m_stop = false;
        start();
    }
}

void YInputParser::stopParser()
{
    if (isRunning()) {
        m_stop = true;
        wait();
    }
}

void YInputParser::run()
{
    char    error_str[128] = {0};
    QString error;

    /* set up timeout to avoid dead waiting in tcp */
    AVDictionary *options = NULL;
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    av_dict_set(&options, "stimeout", "5000000", 0);

    AVFormatContext *fc = avformat_alloc_context();
    fc->max_delay = 500000;

    int ret = avformat_open_input(&fc, m_url.toStdString().c_str(), NULL, &options);
    if (ret < 0) {
        av_make_error_string(error_str, sizeof(error_str), ret);
        error.append(error_str);
        emit signal_parseError(m_stream_id, error);
        if (nullptr != options) {
            av_dict_free(&options);
        }
        return;
    }

    if (nullptr != options) {
        av_dict_free(&options);
    }

    ret = avformat_find_stream_info(fc, NULL);
    if (ret < 0) {
        avformat_close_input(&fc);
        av_make_error_string(error_str, sizeof(error_str), ret);
        error.append(error_str);
        emit signal_parseError(m_stream_id, error);
        return;
    }

    int video_stream_index = -1;
    for (unsigned int i = 0; i < fc->nb_streams; i++) {
        if (AVMEDIA_TYPE_VIDEO == fc->streams[i]->codecpar->codec_type) {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index < 0) {
        avformat_close_input(&fc);
        error.append("Failed to find video stream!");
        emit signal_parseError(m_stream_id, error);
        return;
    }

    AVCodecParameters *codec_par = fc->streams[video_stream_index]->codecpar;
    YDecoder* decoder = createDecoder(codec_par);

    /* read packet */
    AVPacket *pkt = av_packet_alloc();
    while (av_read_frame(fc, pkt) >= 0 && !m_stop)
    {
        if (pkt->stream_index == video_stream_index) {
            YSharedPacket *ypkt = getFreePacket();
            if (ypkt) {
                ypkt->copy(pkt->data, pkt->size);
                ypkt->ref();
                emit signal_packetReady(ypkt);
            }
        }
        av_packet_unref(pkt);
    }

    emit signal_packetReady(nullptr);
    releaseDecoder(decoder);

    for (auto item : ml_packet) {
        while (item->isInUse()) {
            msleep(20);
        }

        delete item;
    }

    /* parser is done and free resource */
    av_packet_free(&pkt);
    avformat_close_input(&fc);

    emit signal_parseDone(m_stream_id);
}

YDecoder* YInputParser::createDecoder(AVCodecParameters *codec_par)
{
    YDecoder* decoder = new YDecoder(m_stream_id, this);
    if (AV_CODEC_ID_H264 == codec_par->codec_id
        || AV_CODEC_ID_HEVC == codec_par->codec_id
        || AV_CODEC_ID_JPEG2000 == codec_par->codec_id) {
//        decoder->createWorker(YDecoder::YDecoderType_Nvcuvid);
        decoder->createWorker(YDecoder::YDecoderType_Ffmpeg);
    } else {
        decoder->createWorker(YDecoder::YDecoderType_Ffmpeg);
    }
    decoder->createFrameBuffer(YDataType_Device);

    connect(this, SIGNAL(signal_setupDecoder(AVCodecParameters*)),
            decoder, SIGNAL(signal_setupDecoder(AVCodecParameters*)), Qt::QueuedConnection);
    connect(this, SIGNAL(signal_packetReady(YSharedPacket*)),
            decoder, SIGNAL(signal_packetReady(YSharedPacket*)), Qt::QueuedConnection);

    connect(decoder, SIGNAL(signal_frameReady(int,YSharedFrame*)),
            this, SIGNAL(signal_frameReady(int,YSharedFrame*)), Qt::QueuedConnection);

    decoder->startDecoder();
    emit signal_setupDecoder(codec_par);

    return decoder;
}

void YInputParser::releaseDecoder(YDecoder* decoder)
{
    decoder->stopDecoder();
    delete decoder;
}

YSharedPacket* YInputParser::getFreePacket()
{
    YSharedPacket* pkt = nullptr;
    for (auto it : ml_packet) {
        if (!it->isInUse()) {
            pkt = it;
            break;
        }
    }

    if (nullptr == pkt) {
        pkt = new YSharedPacket();
        ml_packet.append(pkt);
        static const int MAX_BUFFER_SIZE = 32;
        if (ml_packet.size() > MAX_BUFFER_SIZE) {
            YLOG(Module_Videoio, Severity_WARNING) << "The packet buffer size reaches "<< ml_packet.size()
                << "(max size is 32) in parser-->" << m_stream_id << ", url = " << m_url.toStdString();
        }
    }

    return pkt;
}
