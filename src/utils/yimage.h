





#ifndef YIMAGE_H
#define YIMAGE_H

typedef enum enYDataType {
    YDataType_Host,
    YDataType_HostPaged,
    YDataType_Device
} YDataType;

typedef enum enYPixelFormat {
    YPixelFormat_BGR,
    YPixelFormat_RGB,
    YPixelFormat_BGRA,
    YPixelFormat_RGBA,
    YPixelFormat_YUV420P,
    YPixelFormat_YUV422P,
    YPixelFormat_YUV444P
} YPixelFormat;

class YImage
{
public:
    explicit YImage(YDataType data_type = YDataType_Host, int device = 0);
    virtual ~YImage();

    bool resize(int w, int h, YPixelFormat format);
    bool copy(YImage* src);
    void copyFromHost(void* data, int linesize);

    inline long getPixelWidth() { return m_width; }
    inline long getPixelHeight() { return m_width; }
    static long unsigned int getDataWidth(int w, YPixelFormat format);
    static long unsigned int getDataHeight(int h, YPixelFormat format);
    inline YPixelFormat getPixelFormat() { return m_format; }
    inline long getLinesize() { return m_linesize; }
    inline void* getData() { return m_data; }
    inline int getDeviceId() { return m_device_id; }
    inline YDataType getDataType() { return m_data_type; }

protected:
    long m_width;
    long m_height;
    YPixelFormat m_format;
    unsigned long m_linesize;
    void* m_data;
    int m_device_id;

    YDataType m_data_type;

    void setDefault();

    bool allocData(int w, int h, YPixelFormat format);
    void release();

private:
    /* disable copy */
    YImage& operator=(const YImage&) = delete;
    YImage(const YImage&) = delete;
};

#endif // YIMAGE_H
