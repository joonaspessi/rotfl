#ifndef THREADREADER_H
#define THREADREADER_H

#include <QThread>

namespace CROI {
class PosixSerial;
}

class ThreadReader : public QThread
{
    Q_OBJECT
public:
    //explicit ThreadReader(QObject *parent = 0);
    ThreadReader(CROI::PosixSerial *ps, QObject *parent = 0);
    virtual ~ThreadReader();
    void run();
    bool Stop;

signals:
    void serialread();

public slots:

private:
    CROI::PosixSerial *ps_;

};

#endif // THREADREADER_H
