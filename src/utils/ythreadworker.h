





#ifndef YTHREADWORKER_H
#define YTHREADWORKER_H


#include <QObject>

class YThreadWorker : public QObject
{
    Q_OBJECT
public:
    explicit YThreadWorker() {
        connect(this, SIGNAL(signal_createWorker()), this, SLOT(slot_createWorker()), Qt::QueuedConnection);
        connect(this, SIGNAL(signal_releaseWorker()), this, SLOT(slot_releaseWorker()), Qt::QueuedConnection);
    }
    virtual ~YThreadWorker() {}

    /* create and release context of worker in background thread */
    void createAsync();
    void releaseAsync();

signals:
    void signal_workerCreated(YThreadWorker *worker);

signals:
    void signal_createWorker();
    void signal_releaseWorker();

protected slots:
    void slot_createWorker();
    void slot_releaseWorker();

protected:
    virtual void create() = 0;
    virtual void release() = 0;
};

#endif // YTHREADWORKER_H
