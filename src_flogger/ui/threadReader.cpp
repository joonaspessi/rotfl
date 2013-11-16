#include <iostream>
#include "threadReader.h"
#include "croi/posixSerial.h"

ThreadReader::ThreadReader(Croi::PosixSerial *ps, QObject *parent) :
    QThread(parent),
    ps_(ps)
{}

ThreadReader::~ThreadReader() {

}

void ThreadReader::run() {

    while(true) {
        std::string buf;
        ps_->readSerial(buf);
        //std::cout << "huhuh tulee tavaraa " << buf <<  std::endl;
    }

}


