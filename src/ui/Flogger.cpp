
#include "Flogger.h"

Flogger::Flogger()
    :
      outFile("Flogger.txt")
{
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    ts = new QTextStream(&outFile);
}

