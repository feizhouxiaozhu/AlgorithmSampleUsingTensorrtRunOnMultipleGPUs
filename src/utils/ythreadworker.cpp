





#include "ythreadworker.h"

void YThreadWorker::createAsync()
{
    emit signal_createWorker();
}

void YThreadWorker::releaseAsync()
{
    emit signal_releaseWorker();
}

void YThreadWorker::slot_createWorker()
{
    create();
    emit signal_workerCreated(this);
}

void YThreadWorker::slot_releaseWorker()
{
    release();
//    disconnect(this, nullptr, nullptr, nullptr);
}
