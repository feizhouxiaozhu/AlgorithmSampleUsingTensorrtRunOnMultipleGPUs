





#ifndef NVDEVICEMANAGER_H
#define NVDEVICEMANAGER_H

#include <cuda.h>
#include <cuda_runtime.h>

#include <QMap>
#include <QList>
#include <QMutex>

class NvDevice
{
public:
    explicit NvDevice(int device, QString card_name);
    ~NvDevice();

    bool createCtx();
    CUcontext getCtx();
    int getDeivceId();

    void pushCtx();
    void popCtx();

    QString getCardName();
private:
    CUcontext m_ctx;
    int  m_device;
    QString   m_card_name;
    int m_shared_count;

    QMutex *m_mutex;
};

class NvDeviceManager
{
public:
    ~NvDeviceManager();

    int getDeviceCount();
    NvDevice* requireDeviceWithMemory(float mem);
    NvDevice* requireDeviceByDeviceId(int device);

    static NvDeviceManager *getInstance() {
        return m_instance;
    }
private:
    NvDeviceManager();
    NvDeviceManager(const NvDeviceManager &) = delete;
    NvDeviceManager& operator=(const NvDeviceManager&) = delete;

    static NvDeviceManager *m_instance;

private:
    QMap<int, NvDevice*> mmap_device;
};

#endif // NVDEVICEMANAGER_H
