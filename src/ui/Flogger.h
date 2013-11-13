#ifndef FLOGGER_H
#define FLOGGER_H

#include <qfile.h>
#include <qtextstream.h>

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




#endif // FLOGGER_H
