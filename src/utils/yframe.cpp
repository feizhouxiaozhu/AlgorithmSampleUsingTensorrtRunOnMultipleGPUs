





#include <cuda.h>
#include <cuda_runtime.h>

#include <memory.h>
#include <assert.h>

#include "yframe.h"

YFrame::YFrame(enDataType datatype, int device) {
    width       = 0;
    height      = 0;
    format      = NvPixFormat_None;
    data_capacity = 0;
    cpu_data    = nullptr;
    gpu_data    = nullptr;
    data_type   = datatype;
    device_id   = device;
}

YFrame::~YFrame()
{
    freeData();
}

void YFrame::copy(YFrame *src)
{
    assert(NULL != src);
//    assert(src->device_id == device_id);

    resize(src->width, src->height, src->format);
    int data_height     = getDataHeight(src->height, src->format);
    int src_data_pitch  = getDataPitch(src->width, src->format);
    int dst_data_pitch  = getDataPitch(width, format);
    if (NvDataType_CPU == data_type) {
        if (NvDataType_CPU == src->data_type) {
            cudaMemcpy2D(cpu_data, dst_data_pitch, src->data(), src_data_pitch,
                         getDataLinesize(width, format), data_height, cudaMemcpyHostToHost);
        } else {
            cudaMemcpy2D(gpu_data, dst_data_pitch, src->data(), src_data_pitch,
                         getDataLinesize(width, format), data_height, cudaMemcpyDeviceToHost);
        }
    } else {
        if (NvDataType_CPU == src->data_type) {
            cudaMemcpy2D(cpu_data, dst_data_pitch, src->data(), src_data_pitch,
                         getDataLinesize(width, format), data_height, cudaMemcpyHostToDevice);
        } else {
            cudaMemcpy2D(gpu_data, dst_data_pitch, src->data(), src_data_pitch,
                         getDataLinesize(width, format), data_height, cudaMemcpyDeviceToDevice);
        }
    }
}

void YFrame::copy(void* data, int w, int h, int pitch, enNvFormat fmt)
{
    resize(w, h, fmt);
    int data_height     = getDataHeight(h, fmt);
    int dst_data_pitch  = getDataPitch(w, fmt);
    if (NvDataType_CPU == data_type) {
        cudaMemcpy2D(cpu_data, dst_data_pitch, data, pitch,
                     getDataLinesize(width, format), data_height, cudaMemcpyHostToHost);
    } else {
        cudaMemcpy2D(gpu_data, dst_data_pitch, data, pitch,
                     getDataLinesize(width, format), data_height, cudaMemcpyHostToDevice);
    }
}

bool YFrame::resize(int w, int h, enNvFormat fmt)
{
    if (w <= 0 || h <= 0 || fmt <= NvPixFormat_None || fmt >= NvPixFormat_Max ) {
        return false;
    }

    int dst_size  = getDataPitch(w, fmt) * getDataHeight(h, fmt);
    if (data_capacity < dst_size) {
        if(!allocData(dst_size)) {
            return false;
        }
    }

    width     = w;
    height    = h;
    format    = fmt;
    return true;
}

int YFrame::getDataLinesize(int w, enNvFormat fmt)
{
    int data_size = 0;
    switch (fmt) {
    case NvPixFormat_BGR:
    case NvPixFormat_RGB:
        data_size = w * 3;
        break;
    case NvPixFormat_ABGR:
    case NvPixFormat_BGRA:
        data_size = w * 4;
        break;
    case NvPixFormat_YUV420P:
    case NvPixFormat_YUV444P:
        data_size = w;
        break;
    }

    return data_size;
}

int YFrame::getDataPitch(int w, enNvFormat fmt)
{
    int align = 8;
    if (NvDataType_GPU == data_type) {
        align = 256;
    }

    int pitch = 0;
    switch (fmt) {
    case NvPixFormat_BGR:
    case NvPixFormat_RGB:
        pitch = (w * 3 + align - 1) / align * align;
        break;
    case NvPixFormat_ABGR:
    case NvPixFormat_BGRA:
        pitch = (w * 4 + align - 1) / align * align;
        break;
    case NvPixFormat_YUV420P:
        pitch = (w + align - 1) / align * align;
        break;
    case NvPixFormat_YUV444P:
        pitch = (w + align - 1) / align * align;
        break;
    }

    return pitch;
}

int YFrame::getDataHeight(int h, enNvFormat fmt)
{
    int data_height = 0;
    switch (fmt) {
    case NvPixFormat_BGR:
    case NvPixFormat_RGB:
    case NvPixFormat_ABGR:
    case NvPixFormat_BGRA:
        data_height = h;
        break;
    case NvPixFormat_YUV420P:
        data_height = h + (h + 1) / 2;
        break;
    case NvPixFormat_YUV444P:
        data_height = h * 3;
        break;
    }

    return data_height;
}

bool YFrame::allocData(int size)
{
    freeData();
    if (NvDataType_CPU == data_type) {
        cpu_data = malloc(size);
        memset(cpu_data, 0, size);
    } else {
        cudaMalloc(&gpu_data, size);
        cudaMemset(gpu_data, 0, size);
    }

    if (nullptr == cpu_data && nullptr == gpu_data) {
        return false;
    }

    data_capacity = size;
    return true;
}

void YFrame::freeData()
{
    data_capacity = 0;
    if (nullptr != cpu_data) {
        free(cpu_data);
        cpu_data = nullptr;
    }

    if (nullptr != gpu_data) {
        cudaFree(gpu_data);
        gpu_data = 0;
    }
}
