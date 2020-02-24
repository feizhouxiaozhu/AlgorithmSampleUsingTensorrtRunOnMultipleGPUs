





#include <assert.h>

#include "ydecoder.h"
#include "yffmpegdecoderworker.h"
#include "utils/ylogger.h"
#include "utils/nvdevicemanager.h"

YDecoderWorker::YDecoderWorker(int stream_id)
{
    m_stream_id = stream_id;
    m_timer = new QTimer(this);
}

YDecoderWorker::~YDecoderWorker()
{
    delete m_timer;
}

void YDecoderWorker::slot_setupFrameBuffer(const QList<YSharedFrame*>& frame_list)
{
    ml_free_frame = frame_list;
}

void YDecoderWorker::create()
{
    m_timer->start(40);
    connect(m_timer, SIGNAL(timeout()),
            this, SLOT(slot_outputReadyFrame()), Qt::QueuedConnection);
}

void YDecoderWorker::release()
{
    disconnect(m_timer, SIGNAL(timeout()), this, SLOT(slot_outputReadyFrame()));
    m_timer->stop();
}

void YDecoderWorker::slot_outputReadyFrame()
{
    if (ml_ready_frame.isEmpty()) {
        YLOG(Module_Videoio, Severity_WARNING) << "No ready frame found!";
        return;
    }

    auto item = ml_ready_frame.takeFirst();
    emit signal_frameReady(m_stream_id, item);
}

YSharedFrame* YDecoderWorker::getFreeFrame()
{
    YSharedFrame* frame = nullptr;
    for (int i = 1; i < ml_free_frame.size(); i++) {
        if (!ml_free_frame.at(i)->isInUse()) {
            frame = ml_free_frame.at(i);
            break;
        }
    }

    if (nullptr == frame) {
        YLOG(Module_Videoio, Severity_WARNING) << "No free frame found!";
    }

    return frame;
}

YDecoder::YDecoder(int id, QObject *parent) : QThread(parent)
{
    m_id     = id;
    m_width  = 1280;
    m_height = 720;
    m_format = YPixelFormat_BGR;
    m_worker = nullptr;
    m_device = nullptr;
}

YDecoder::~YDecoder()
{
    delete m_worker;
    releaseFrameBuffer();
}

void YDecoder::createWorker(YDecoderType decoder_type)
{
    if (YDecoderType_Ffmpeg == decoder_type) {
        m_worker = new YFfmpegDecoderWorker(m_id);
    } else if (YDecoderType_Nvcuvid == decoder_type) {
        m_worker = nullptr;
    }

    connect(this, SIGNAL(signal_setupFrameBuffer(QList<YSharedFrame*>)),
            m_worker, SLOT(slot_setupFrameBuffer(QList<YSharedFrame*>)), Qt::QueuedConnection);
    connect(this, SIGNAL(signal_setupDecoder(AVCodecContext*)),
            m_worker, SLOT(slot_setupDecoder(AVCodecContext*)), Qt::QueuedConnection);
    connect(this, SIGNAL(signal_packetReady(YSharedPacket*)),
            m_worker, SLOT(slot_packetReady(YSharedPacket*)), Qt::QueuedConnection);
    connect(m_worker, SIGNAL(signal_frameReady(int,YSharedFrame*)),
            this, SIGNAL(signal_frameReady(int,YSharedFrame*)), Qt::QueuedConnection);
    m_worker->moveToThread(this);
}

void YDecoder::setupOutputFormat(int w, int h, YPixelFormat fmt)
{
    m_width  = w;
    m_height = h;
    m_format = fmt;
}

void YDecoder::createFrameBuffer(YDataType data_type, int buffer_count)
{
    if (YDataType_Device == data_type) {
        int size = buffer_count * YImage::getDataWidth(m_width, m_format) * YImage::getDataHeight(m_height, m_format);
        m_device = NvDeviceManager::getInstance()->requireDeviceWithMemory(size);
        m_device->pushCtx();
    }

    for (int i = 0; i < buffer_count; i++) {
        YSharedFrame* frame = new YSharedFrame(data_type);
        frame->resize(m_width, m_height, m_format);
        ml_frame.append(frame);
    }

    if (YDataType_Device == data_type) {
        m_device->popCtx();
    }
}

void YDecoder::startDecoder()
{
    if (isRunning()) {
        YLOG(Module_Videoio, Severity_WARNING) << "The decoder has been already started!";
        return;
    }

    start();
    m_worker->createAsync();
    emit signal_setupFrameBuffer(ml_frame);
}

void YDecoder::stopDecoder()
{
    if (isRunning()) {
        m_worker->releaseAsync();
        quit();
        wait();
    }
}

void YDecoder::releaseFrameBuffer()
{
    if (m_device) {
        m_device->pushCtx();
    }

    for (auto item : ml_frame) {
        delete item;
    }

    if (m_device) {
        m_device->popCtx();
        m_device = nullptr;
    }
}
