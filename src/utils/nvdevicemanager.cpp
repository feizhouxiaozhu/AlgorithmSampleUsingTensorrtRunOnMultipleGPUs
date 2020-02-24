




#include <dlfcn.h>
#include <unistd.h>

#include "utils/ylogger.h"
#include "nvdevicemanager.h"

NvDeviceManager* NvDeviceManager::m_instance = new NvDeviceManager();

NvDevice::NvDevice(int device, QString card_name)
{
    m_ctx       = nullptr;
    m_device    = device;
    m_card_name = card_name;
    m_shared_count = 0;
    m_mutex = new QMutex();
}

NvDevice::~NvDevice()
{
    delete m_mutex;
    if (m_ctx) {
        cuCtxDestroy(m_ctx);
    }
}

bool NvDevice::createCtx()
{
    CUresult ret = cuCtxCreate(&m_ctx, 0, m_device);
    if (CUDA_SUCCESS != ret) {
        YLOG(Module_Videoio, Severity_WARNING) << "Failed to create cuda context on device " << m_device << "-->" << ret << "!";
        return false;
    }

    CUcontext ctx = NULL;
    cuCtxPopCurrent(&ctx);
    return true;
}

CUcontext NvDevice::getCtx()
{
    return m_ctx;
}

int NvDevice::getDeivceId()
{
    return m_device;
}

void NvDevice::pushCtx()
{
    m_mutex->lock();
    CUresult ret = cuCtxPushCurrent(m_ctx);
    while (CUDA_SUCCESS != ret) {
        usleep(10 * 1000);
        ret = cuCtxPushCurrent(m_ctx);
    }
    m_shared_count++;
    m_mutex->unlock();
}

void NvDevice::popCtx()
{
    m_mutex->lock();
    CUcontext ctx = NULL;
    cuCtxPopCurrent(&ctx);

    m_shared_count--;
    m_mutex->unlock();
}

QString NvDevice::getCardName()
{
    return m_card_name;
}

NvDeviceManager::NvDeviceManager()
{
    cudaError_t ret = cudaSuccess;

    /* init driver api to push and pop ctx */
    if (CUDA_SUCCESS != cuInit(0)) {
        YLOG(Module_Videoio, Severity_ERROR) << "Failed to init nvidia driver!";
        return;
    }

    int device_count = 0;
    ret = cudaGetDeviceCount(&device_count);
    if (cudaSuccess != ret) {
        YLOG(Module_Videoio, Severity_ERROR) << "Failed to detect nvidia device-->" << ret << "!";
        return;
    }

    for (int i = 0; i < device_count; i++) {
        cudaDeviceProp deviceProp;
        ret = cudaGetDeviceProperties(&deviceProp, i);
        if (cudaSuccess == ret)
        {
            /* Nvenc capabilities are available on GPUs with version more than 3.0
             * TensorRt requires major version more than 5.0 */
            if (deviceProp.major >= 5) {
                YLOG(Module_Videoio, Severity_INFO) << "Nvidia card info:";
                YLOG(Module_Videoio, Severity_INFO) << "    Nvidia card name is " << deviceProp.name;
                YLOG(Module_Videoio, Severity_INFO) << "    Compute capability is " << deviceProp.major << "-" << deviceProp.minor;

                NvDevice* nv_device =  new NvDevice(i, deviceProp.name);
                if (nv_device->createCtx()) {
                    mmap_device.insert(i, nv_device);
                } else {
                    delete nv_device;
                }
            }
        }
    }
}

NvDeviceManager::~NvDeviceManager()
{
    for (auto item : mmap_device) {
        delete item;
    }
}

int NvDeviceManager::getDeviceCount()
{
    return mmap_device.size();
}

NvDevice* NvDeviceManager::requireDeviceWithMemory(float mem)
{
    NvDevice *nv_device = nullptr;
    size_t mem_free, mem_total, mem_free_max = 0;
    for (auto item : mmap_device) {
        item->pushCtx();
        cudaMemGetInfo(&mem_free, &mem_total);
        if (mem_free_max < mem_free && mem_free > mem) {
            nv_device    = item;
            mem_free_max = mem_free;
        }
        item->popCtx();
    }

    return nv_device;
}

NvDevice* NvDeviceManager::requireDeviceByDeviceId(int device)
{
    NvDevice *nv_device = nullptr;
    if (mmap_device.contains(device)) {
        nv_device = mmap_device.value(device);
    }

    return nv_device;
}
