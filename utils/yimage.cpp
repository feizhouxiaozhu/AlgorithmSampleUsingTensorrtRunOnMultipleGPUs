






#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

#include "yimage.h"
#include "utils/ylogger.h"


YImage::YImage(YDataType data_type, int device)
{
    m_device    = device;
    m_data_type = data_type;
    setDefault();
}

YImage::~YImage()
{
    release();
}

int YImage::getBytesPerPixel(int format) {
    int bpp = 0;
    switch (format) {
        case YPixelFormat_BGR:
        case YPixelFormat_RGB:
            bpp = 3;
        break;
        case YPixelFormat_BGRA:
        case YPixelFormat_RGBA:
            bpp = 4;
        break;
        case YPixelFormat_YUV420P:
        case YPixelFormat_YUV422P:
        case YPixelFormat_YUV444P:
            bpp = 1;
        break;
    default:
        YLOG(Module_Utils, WARNING) << "Unknown pixel format-->" << m_format << "!";
    }

    return bpp;
}

bool YImage::resize(int w, int h, int format)
{
    if (getPlaneCount(m_format) != getPlaneCount(format)
        || m_linesize < w * getBytesPerPixel(format)) {

        release();
        if (allocData(w, h, format)) {
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
    ret = resize(src->getWidth(), src->getHeight(), src->getFormat());
    if (!ret) {
        return false;
    }

    int height = getTotalHeight(m_height, m_format);
#ifdef EnablePitch
    if (YDataType_Device == m_data_type) {
        if (YDataType_Device == src->m_data) {
            cudaMemcpy2D(m_data, m_linesize, src->getData(), src->getLinesize(),
                         m_width * getBytesPerPixel(m_format), height, cudaMemcpyDeviceToDevice);
        } else {
            cudaMemcpy2D(m_data, m_linesize, src->getData(), src->getLinesize(),
                         m_width * getBytesPerPixel(m_format), height, cudaMemcpyHostToDevice);
        }
    } else {
        if (YDataType_Device == src->m_data) {
            cudaMemcpy2D(m_data, m_linesize, src->getData(), src->getLinesize(),
                         m_width * getBytesPerPixel(m_format), height, cudaMemcpyDeviceToHost);
        } else {
            cudaMemcpy2D(m_data, m_linesize, src->getData(), src->getLinesize(),
                         m_width * getBytesPerPixel(m_format), height, cudaMemcpyHostToHost);
        }
    }
#else
    if (YDataType_Device == m_data_type) {
        if (YDataType_Device == src->getDataType()) {
            cudaMemcpy(m_data, src->getData(), height * m_linesize, cudaMemcpyDeviceToDevice);
        } else {
            cudaMemcpy(m_data, src->getData(), height * m_linesize, cudaMemcpyHostToDevice);
        }
    } else {
        if (YDataType_Device == src->getDataType()) {
            cudaMemcpy(m_data, src->getData(), height * m_linesize, cudaMemcpyDeviceToHost);
        } else {
            cudaMemcpy(m_data, src->getData(), height * m_linesize, cudaMemcpyHostToHost);
        }
    }
#endif

    cudaError_t err = cudaGetLastError();
    if (cudaSuccess != err) {
        YLOG(Module_Utils, ERROR) << "Failed to excute cuda functions!";
        return false;
    }

    return  true;
}

int YImage::getALignment()
{
    int align = 8;
    if (YDataType_Device == m_data_type) {
        /* TODO: study texture alignment usage on cuda device and determine alignment on cuda device
         * Now just align to 8 default as host
         */
        align = 8;
    }

    return align;
}

int YImage::getPlaneCount(int format)
{
    int planes = 0;
    switch (format) {
        case YPixelFormat_BGR:
        case YPixelFormat_RGB:
        case YPixelFormat_BGRA:
        case YPixelFormat_RGBA:
            planes = 1;
        break;
        case YPixelFormat_YUV420P:
        case YPixelFormat_YUV422P:
        case YPixelFormat_YUV444P:
            planes = 3;
        break;
    default:
        YLOG(Module_Utils, WARNING) << "Unknown pixel format-->" << m_format << "!";
    }

    return planes;
}

int YImage::getTotalHeight(int h, int format)
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
        YLOG(Module_Utils, WARNING) << "Unknown pixel format-->" << m_format << "!";
    }

    return height;
}

void YImage::setDefault()
{
    m_width    = 0;
    m_height   = 0;
    m_format   = YPixelFormat_None;
    m_linesize = 0;
    m_data     = nullptr;
    m_align    = getALignment();
}

bool YImage::allocData(int w, int h, int format)
{
    m_linesize = (w * getBytesPerPixel(format) + m_align - 1) / m_align * m_align;
    int size = m_linesize * getTotalHeight(h, format);
    if (YDataType_Host == m_data_type) {
        m_data = malloc(size);
    } else if(YDataType_HostPaged == m_data_type) {
        cudaHostAlloc(&m_data, size, cudaHostAllocPortable);
    } else {
        cudaMalloc(&m_data, size);
    }

    if (nullptr == m_data) {
        YLOG(Module_Utils, ERROR) << "Insufficient memory to alloc " << size << " data!";
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
