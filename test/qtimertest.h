





#ifndef QTIMERTEST_H
#define QTIMERTEST_H

#include <QTimer>
#include <QObject>

class QTimerTest : public QObject
{
    Q_OBJECT
public:
    QTimerTest(QObject *parent = nullptr);

protected slots:
    void slot_test();

private:
    QTimer m_timer;
};

#endif // QTIMERTEST_H
