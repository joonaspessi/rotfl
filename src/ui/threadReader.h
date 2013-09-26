#ifndef THREADREADER_H
#define THREADREADER_H

#include <QThread>

namespace Croi {
class PosixSerial;
}

class ThreadReader : public QThread
{
    Q_OBJECT
public:
    //explicit ThreadReader(QObject *parent = 0);
    ThreadReader(Croi::PosixSerial *ps, QObject *parent = 0);
    virtual ~ThreadReader();
    void run();
    bool Stop;

signals:
    void serialread();

public slots:

private:
    Croi::PosixSerial *ps_;

};

#endif // THREADREADER_H
