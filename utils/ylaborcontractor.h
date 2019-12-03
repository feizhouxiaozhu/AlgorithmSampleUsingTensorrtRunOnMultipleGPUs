





#ifndef YLABORCONTRACTOR_H
#define YLABORCONTRACTOR_H

#include <QMap>
#include <QObject>
#include <QThread>

class YJobStatisticInfo
{
public:
    YJobStatisticInfo() {
        m_required_task  = 0;
        m_responsed_task = 0;
    }

    int m_required_task;
    int m_responsed_task;

    bool operator <(const YJobStatisticInfo& job_info)
    {
        float percent  = m_responsed_task * 1.0 / m_required_task;
        float percent2 = job_info.m_responsed_task * 1.0 / job_info.m_required_task;
        return percent < percent2;
    }

    bool operator >(const YJobStatisticInfo& job_info)
    {
        float percent  = m_responsed_task * 1.0 / m_required_task;
        float percent2 = job_info.m_responsed_task * 1.0 / job_info.m_required_task;
        return percent > percent2;
    }
};

class YWorker : public QObject
{
    Q_OBJECT
public:
    explicit YWorker(int capacity);
    virtual ~YWorker() {}

    void createWorker();
    bool isScheduled();
    bool isAvailable();
    void dispatchJob();
    void removeJob();
    void releaseWorker();

protected:
    virtual void create() = 0;
    virtual void release() = 0;

signals:
    void signal_createWorker();
    void signal_workerCreated(YWorker *worker);
    void signal_releaseWorker();

protected slots:
    void slot_createWorker();
    void slot_releaseWorker();

private:
    int m_usage;
    int m_capacity;
};

/* YLaborContractor schedules jobs and arranges workers running on background thread to excute tasks in jobs.
 * The concurrent jobs scheduled are limited by background worker count, which may be determined by system resources.
 * The implemention of YLaborContractor here only supplies asynchronized interfaces using Qt signals, it is the
 * caller's responsibility to listen these signals emitted on task done.
 */
class YLaborContractor
{
public:
    explicit YLaborContractor(int worker_count);
    virtual ~YLaborContractor() {}

    /* Here implements a simple rule with assumption that tasks in all jobs can be excuted by any worker.
     * The subclass should override this function for other rules complied with tasks with same job_id and workers.
     */
    virtual bool requireWorkerForJob(int job_id);
    virtual bool releaseWorkerForJob(int job_id);

protected:
    int m_worker_count;
    QList<YWorker*> ml_worker;
    QMap<YWorker*, QThread*> mmap_free_worker;
    QMap<YWorker*, QThread*> mmap_busy_worker;

    QMap<int, YJobStatisticInfo> mmap_job_info;
};

#endif // YLABORCONTRACTOR_H
