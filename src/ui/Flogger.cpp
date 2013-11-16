#include "Flogger.h"


Flogger flog;

Flogger::Flogger() :
    outFile("Flogger.txt")
{
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    ts = new QTextStream(&outFile);
}

