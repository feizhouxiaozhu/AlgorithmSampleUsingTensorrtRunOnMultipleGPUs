




#ifndef YIMAGE_H
#define YIMAGE_H


typedef enum enYDataType {
    YDataType_Host,
    YDataType_HostPaged,
    YDataType_Device
} YDataType;

typedef enum enYPixelFormat {
    YPixelFormat_None,
    YPixelFormat_BGR,
    YPixelFormat_RGB,
    YPixelFormat_BGRA,
    YPixelFormat_RGBA,
    YPixelFormat_YUV420P,
    YPixelFormat_YUV422P,
    YPixelFormat_YUV444P,
    YPixelFormat_Max
} YPixelFormat;

class YImage
{
public:
    explicit YImage(YDataType data_type = YDataType_Host, int device = 0);
    ~YImage();

    inline int getWidth() { return m_width; }
    inline int getHeight() { return m_width; }
    inline int getFormat() { return m_format; }
    inline int getLinesize() { return m_linesize; }
    inline void* getData() { return m_data; }
    inline int getDevice() { return m_device; }
    inline YDataType getDataType() { return m_data_type; }

    bool resize(int w, int h, int format);
    bool copy(YImage* src);

    /* disable copy */
    YImage& operator=(const YImage&) = delete;
    YImage(const YImage&) = delete;

protected:
    int m_width;
    int m_height;
    int m_format;
    int m_linesize;
    void* m_data;
    int m_device;

    YDataType m_data_type;

    int m_align;
    int getALignment();
    int getPlaneCount(int format);
    int getTotalHeight(int h, int format);
    /* get bytes per pixel on one plane */
    int getBytesPerPixel(int format);

    void setDefault();

    bool allocData(int w, int h, int format);
    void release();
};

#endif // YIMAGE_H
