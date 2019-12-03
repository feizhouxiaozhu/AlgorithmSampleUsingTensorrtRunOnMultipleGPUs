





#include "ylogger.h"
#include "ylaborcontractor.h"

YWorker::YWorker(int capacity)
{
    m_usage    = 0;
    m_capacity = capacity;
    connect(this, SIGNAL(signal_createWorker()), this, SLOT(slot_createWorker()), Qt::QueuedConnection);
    connect(this, SIGNAL(signal_releaseWorker()), this, SLOT(slot_releaseWorker()), Qt::QueuedConnection);
}

void YWorker::createWorker()
{
    emit signal_createWorker();
}

bool YWorker::isScheduled()
{
    return m_usage;
}

bool YWorker::isAvailable()
{
    return m_usage < m_capacity;
}

void YWorker::dispatchJob()
{
    m_usage++;
}

void YWorker::removeJob()
{
    m_usage--;
}

void YWorker::releaseWorker()
{
    emit signal_releaseWorker();
}

void YWorker::slot_createWorker()
{
    createWorker();
    emit signal_workerCreated(this);
}

void YWorker::slot_releaseWorker()
{
    releaseWorker();
}

YLaborContractor::YLaborContractor(int worker_count)
{
    m_worker_count = worker_count;
}

bool YLaborContractor::requireWorkerForJob(int job_id)
{
    if (mmap_job_info.contains(job_id)) {
        YLOG(Module_Utils, WARNING) << "The job " << job_id << " has been scheduled worker!";
        return false;
    }

    bool dispatched = false;
    for (auto t : ml_worker)
    {
        if (t->isAvailable()) {
            t->dispatchJob();
            dispatched = true;
            break;
        }
    }

    if (!dispatched) {
        YLOG(Module_Utils, WARNING) << "There is no worker can be scheduled for job " << job_id << "!";
        return false;
    }

    YJobStatisticInfo tmp;
    mmap_job_info.insert(job_id, tmp);

    return true;
}

bool YLaborContractor::releaseWorkerForJob(int job_id)
{
    if (!mmap_job_info.contains(job_id)) {
        YLOG(Module_Utils, WARNING) << "The job " << job_id << " has not been scheduled worker!";
        return false;
    }

    bool removed = false;
    for (auto t : ml_worker)
    {
        if (t->isScheduled()) {
            t->removeJob();
            removed = true;
            break;
        }
    }

    if (!removed) {
        YLOG(Module_Utils, WARNING) << "There is no worker scheduled for job " << job_id << "!";
    }

    mmap_job_info.remove(job_id);
    return true;
}
