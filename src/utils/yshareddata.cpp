





#include "yshareddata.h"

YSharedData::YSharedData(QObject *parent) : QObject(parent)
{
    m_mutex = new QMutex();
    m_ref_count = 0;
}

YSharedData::~YSharedData()
{
    delete m_mutex;
}

void YSharedData::ref()
{
    m_ref_count++;
}

void YSharedData::unref()
{
    if (m_ref_count) {
        m_ref_count--;
    }
}

bool YSharedData::isReferenced()
{
    return m_ref_count;
}

void YSharedData::addUser(int user) {
    ml_user.append(user);
}

void YSharedData::removeUser(int user)
{
    ml_user.removeOne(user);
}

bool YSharedData::hasUser()
{
    return ml_user.size();
}

bool YSharedData::isInUse()
{
    return (m_ref_count || ml_user.size());
}

void YSharedData::lock()
{
    m_mutex->lock();
}

void YSharedData::unlock()
{
    m_mutex->unlock();
}
