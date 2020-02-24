






#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

#include "yimage.h"
#include "utils/ylogger.h"


YImage::YImage(YDataType data_type, int device)
{
    m_device_id    = device;
    m_data_type = data_type;
    setDefault();
}

YImage::~YImage()
{
    release();
}

bool YImage::resize(int w, int h, YPixelFormat format)
{
    if (getDataWidth(m_width, m_format) < getDataWidth(w, format)
        || getDataHeight(m_height, m_format) < getDataHeight(h, format)) {

        release();
        if (!allocData(w, h, format)) {
            return false;
        }
    }

    m_width  = w;
    m_height = h;
    m_format = format;

    return true;
}

bool YImage::copy(YImage *src)
{
    bool ret = false;
    ret = resize(src->getPixelWidth(), src->getPixelHeight(), src->getPixelFormat());
    if (!ret) {
        return false;
    }

    cudaMemcpyKind kind = cudaMemcpyDefault;
    if (YDataType_Device == m_data_type) {
        if (YDataType_Device == src->m_data_type) {
            kind = cudaMemcpyDeviceToDevice;
        } else {
            kind = cudaMemcpyHostToDevice;
        }
    } else {
        if (YDataType_Device == src->m_data_type) {
            kind = cudaMemcpyDeviceToHost;
        } else {
            kind = cudaMemcpyHostToHost;
        }
    }
    cudaMemcpy2D(m_data, m_linesize, src->getData(), src->getLinesize(),
                 getDataWidth(m_width, m_format), getDataHeight(m_height, m_format), kind);

    return  true;
}

void YImage::copyFromHost(void* data, int linesize)
{
    cudaMemcpyKind kind = cudaMemcpyDefault;
    if (YDataType_Device == m_data_type) {
        kind = cudaMemcpyHostToDevice;
    } else {
        kind = cudaMemcpyHostToHost;
    }
    cudaMemcpy2D(m_data, m_linesize, data, linesize, getDataWidth(m_width, m_format), getDataHeight(m_height, m_format), kind);
}

long unsigned int YImage::getDataWidth(int w, YPixelFormat format)
{
    int width = 0;
    switch (format) {
        case YPixelFormat_BGR:
        case YPixelFormat_RGB:
            width = w * 3;
        break;
        case YPixelFormat_BGRA:
        case YPixelFormat_RGBA:
            width = w * 4;
        break;
        case YPixelFormat_YUV420P:
        case YPixelFormat_YUV422P:
        case YPixelFormat_YUV444P:
            width = w;
        break;
    default:
        YLOG(Module_Utils, Severity_WARNING) << "Unknown pixel format-->" << format << "!";
    }

    return width;
}

unsigned long YImage::getDataHeight(int h, YPixelFormat format)
{
    int height = 0;
    switch (format) {
        case YPixelFormat_BGR:
        case YPixelFormat_RGB:
        case YPixelFormat_BGRA:
        case YPixelFormat_RGBA:
            height = h;
            break;
        case YPixelFormat_YUV420P:
            height = h + (h + 1) / 2;
            break;
        case YPixelFormat_YUV422P:
            height = h * 2;
            break;
        case YPixelFormat_YUV444P:
            height = h * 3;
            break;
    default:
        YLOG(Module_Utils, Severity_ERROR) << "Unknown pixel format-->" << format << "!";
    }

    return height;
}

void YImage::setDefault()
{
    m_width    = 0;
    m_height   = 0;
    m_format   = YPixelFormat_BGR;
    m_linesize = 0;
    m_data     = nullptr;
}

bool YImage::allocData(int w, int h, YPixelFormat format)
{
    int align = 8;  /* due to address of x64 system */
    m_linesize = (getDataWidth(w, format) + align - 1) / align * align;
    int size = m_linesize * getDataHeight(h, format);
    if (YDataType_Host == m_data_type) {
        m_data = malloc(size);
    } else if(YDataType_HostPaged == m_data_type) {
        cudaHostAlloc(&m_data, size, cudaHostAllocPortable);
    } else {
        cudaMallocPitch(&m_data, &m_linesize, getDataWidth(w, format), getDataHeight(h, format));
    }

    if (nullptr == m_data) {
        YLOG(Module_Utils, Severity_ERROR) << "Insufficient memory to alloc " << size << " data!";
        return false;
    }

    return true;
}

void YImage::release()
{
    if (YDataType_Host == m_data_type) {
        free(m_data);
    } else if (YDataType_HostPaged == m_data_type) {
        cudaFreeHost(m_data);
    } else {
        cudaFree(m_data);
    }

    setDefault();
}
