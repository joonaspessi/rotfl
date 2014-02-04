#ifndef FLOGGER_H
#define FLOGGER_H

#include <QFile>
#include <QTextStream>

class Flogger
{

public:
    virtual ~Flogger() { delete ts; }
    Flogger();
public:
    QTextStream *ts;

private:
    QFile outFile; //("Flogger.txt");

};

extern Flogger flog;


#endif // FLOGGER_H
