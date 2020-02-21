






#ifndef YFRAME_H
#define YFRAME_H

typedef enum enNvFormat {
    NvPixFormat_None,
    NvPixFormat_BGR,
    NvPixFormat_RGB,
    NvPixFormat_ABGR,
    NvPixFormat_BGRA,
    NvPixFormat_YUV420P,
    NvPixFormat_YUV444P,
    NvPixFormat_Max
}NvFormat;

class YFrame
{
public:
    enum enDataType {
        NvDataType_CPU,
        NvDataType_GPU
    };

    explicit YFrame(enDataType datatype = NvDataType_CPU, int device = 0);
    virtual ~YFrame();

    void copy(YFrame *src);
    void copy(void* data, int w, int h, int pitch, enNvFormat fmt);
    bool resize(int w, int h, enNvFormat fmt);

    int getDataLinesize() { return getDataLinesize(width, format); }
    int getDataPitch() { return getDataPitch(width, format); }
    int getDataHeight() { return getDataHeight(height, format); }

    int width;
    int height;
    int format;
    int device_id;

    int frame_index;

    int data_capacity;
    void*       cpu_data;
    void*       gpu_data;
    enDataType  data_type;

    void* data() {
        if (NvDataType_CPU == data_type) {
            return cpu_data;
        } else {
            return gpu_data;
        }
    }

private:
    int getDataLinesize(int w, enNvFormat fmt);
    int getDataPitch(int w, enNvFormat fmt);
    int getDataHeight(int h, enNvFormat fmt);
    bool allocData(int size);
    void freeData();
};

#endif // YFRAME_H
