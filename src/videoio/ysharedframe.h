





#ifndef YSHAREDFRAME_H
#define YSHAREDFRAME_H

#include "utils/yimage.h"
#include "utils/yshareddata.h"

class YSharedPacket : public YSharedData
{
public:
    YSharedPacket() {
        m_size = 0;
        m_capacity = 0;
        m_data = nullptr;
    }

    ~YSharedPacket() {
        release();
    }

    void resize(int size) {
        if (m_capacity < size) {
            release();
            alloc(size);
        }

        m_size = size;
    }

    void copy(void* data, int size) {
        resize(size);
        memcpy(m_data, data, size);
    }

    void* getData() { return m_data; }
    int getSize() { return m_size; }

private:
    void alloc(int size) {
        m_data = malloc(size);
        m_capacity = size;
    }

    void release() {
        if (m_data) {
            free(m_data);
        }
    }

private:
    int   m_size;
    void *m_data;
    int   m_capacity;
};

class YSharedFrame : public YImage, public YSharedData
{
public:
    YSharedFrame(YDataType data_type = YDataType_Host, int device = 0, QObject *parent = nullptr);
    ~YSharedFrame();
};

#endif // YSHAREDFRAME_H
