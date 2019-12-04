





#include "utils/ylogger.h"

#include "qtimertest.h"

QTimerTest::QTimerTest(QObject *parent) : QObject(parent),
    m_timer(this)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(slot_test()), Qt::QueuedConnection);
    m_timer.start(300);
}

void QTimerTest::slot_test()
{
    YLOG(Module_Test, INFO) << "Timer test!";
}
