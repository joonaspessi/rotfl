/****************************************************************************
 ** File
 ** · Flogger.cpp
 **
 ** Version    Author          Date
 ** 0.1        Longchuan       13 / 06 / 2013
 **
 ** Sprint 3,
 ** Task no 18. Add logging of robots
 ** Use Qt model structure to implement basic logging of roombas:
 ** location, command history etc.
 **
 ****************************************************************************/

#include "Flogger.h"

Flogger::Flogger() :
    outFile("Flogger.txt")
{
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    ts = new QTextStream(&outFile);
}

