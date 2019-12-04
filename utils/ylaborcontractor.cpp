





#include "ylogger.h"
#include "ylaborcontractor.h"

YWorker::YWorker(int capacity)
{
    m_usage    = 0;
    m_capacity = capacity;
    connect(this, SIGNAL(signal_createWorker()), this, SLOT(slot_createWorker()), Qt::QueuedConnection);
    connect(this, SIGNAL(signal_releaseWorker()), this, SLOT(slot_releaseWorker()), Qt::QueuedConnection);
}

void YWorker::createAsync()
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

void YWorker::releaseAsync()
{
    emit signal_releaseWorker();
}

void YWorker::slot_createWorker()
{
    createAsync();
    emit signal_workerCreated(this);
}

void YWorker::slot_releaseWorker()
{
    releaseAsync();
}

YLaborContractor::YLaborContractor(int worker_count, QObject *parent) : QObject(parent)
{
    m_initialized = false;
    m_worker_count = worker_count;
    m_worker_created = 0;
    m_name = "LaborContractor";
}

bool YLaborContractor::registerJob(int job_id)
{
    if (mmap_job_info.contains(job_id)) {
        YLOG(Module_Utils, WARNING) << "The job " << job_id << " has been scheduled worker!";
        return false;
    }

    bool dispatched = false;
    for (auto t : mmap_worker.keys())
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

bool YLaborContractor::unregisterJob(int job_id)
{
    if (!mmap_job_info.contains(job_id)) {
        YLOG(Module_Utils, WARNING) << "The job " << job_id << " has not been scheduled worker!";
        return false;
    }

    bool removed = false;
    for (auto t : mmap_worker.keys())
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


void YLaborContractor::init()
{
    for (int i = 0; i < m_worker_count; i++) {

        QString worker_name(m_name);
        worker_name.append(QString::number(i));

        QThread *thread = new QThread();
        thread->setObjectName(worker_name);
        thread->start();

        YWorker *worker = createWorker();
        worker->moveToThread(thread);
        worker->createAsync();

        mmap_worker.insert(worker, thread);
    }
}

void YLaborContractor::release()
{
    while (mmap_worker.size()) {
        auto worker = mmap_worker.firstKey();
        auto thread = mmap_worker.take(worker);
        worker->releaseAsync();
        thread->quit();
        thread->wait();
        delete thread;
        delete worker;
    }
}

void YLaborContractor::slot_workerCreated(YWorker* worker)
{
    auto thread = mmap_worker.value(worker);
    mmap_free_worker.insert(worker, thread);

    m_worker_created++;
    if (m_worker_created == m_worker_count)
    {
        m_initialized = true;
        emit signal_ready(this);
    }
}
