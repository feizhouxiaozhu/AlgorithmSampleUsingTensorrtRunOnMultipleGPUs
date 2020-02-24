





#ifndef YSHAREDDATA_H
#define YSHAREDDATA_H

#include <QList>
#include <QMutex>
#include <QObject>

class YSharedData : protected QObject
{
    Q_OBJECT
public:
    explicit YSharedData(QObject *parent = nullptr);
    virtual ~YSharedData();

    /* use reference count to share data */
    void ref();
    void unref();
    bool isReferenced();

    /* specify users who are using data */
    void addUser(int user);
    void removeUser(int user);
    bool hasUser();

    /* check reference and user all */
    bool isInUse();

    void lock();
    void unlock();

protected:
    QMutex *m_mutex;

private:
    int    m_ref_count;
    QList<int> ml_user;
};

#endif // YSHAREDDATA_H
