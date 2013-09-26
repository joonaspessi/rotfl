/*
 * PosixSerial.cpp
 *
 *  Created on: Sep 20, 2013
 *      Author: Pessi
 */
#include <iostream>
#include "posixSerial.h"

extern "C" {
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
}

namespace CROI {

PosixSerial::PosixSerial() :
    fd_(-1),
    initialized_(false),
    device_(DEFAULT_DEVICE)
{
    settings_.baudrate = B115200;
    settings_.cflag = (CS8 | CLOCAL | CREAD );
    memset(readBuf_, 0, sizeof(readBuf_));
}

PosixSerial::PosixSerial(SerialSettings settings) :
    fd_(-1),
    initialized_(false)
{}

int PosixSerial::writeSerial(const std::string buf) {

    if (!initialized_) {
        if( initializePort() == CROI_ERROR ) {
            return CROI_ERROR;
        }
    }

    int n = write(fd_, buf.c_str(), buf.size());

    std::cout << "lahetetty tavuja: " << n << std::endl;
    if(n != static_cast<int>(buf.size())) {
        return CROI_ERROR;
    }
    return CROI_SUCCESS;
}

int PosixSerial::readSerial(std::string & buf) {

    if(!initialized_) {
        if( initializePort() == CROI_ERROR ) {
            return CROI_ERROR;
        }
    }
    //non blocking read
    fcntl(fd_, F_SETFL, FNDELAY);
    //int origflags = fcntl(fd_, F_GETFL, 0);
    //fcntl(fd_, F_SETFL, origflags & ~O_NONBLOCK);

    int size = read(fd_, readBuf_, MAX_READ);

    fcntl(fd_, F_SETFL, 0);
//    fcntl(fd_, F_SETFL, origflags | O_NONBLOCK);

    if(size == -1){
        return CROI_ERROR;
    }

    //debug
    for(int i = 0; i < size; ++i) {
        std::cout << readBuf_[i];
    }
    std::cout << std::endl;

    std::cout << size << std::endl;
    buf = std::string(readBuf_, readBuf_ + size);

    return CROI_SUCCESS;
}



int PosixSerial::openPort() {

    if(fd_ != -1) {
        return CROI_SUCCESS;
    }

    fd_ = open(device_.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

    //Failed ?
    if(fd_ == -1){
        return CROI_ERROR;
    }

    return CROI_SUCCESS;
}

int PosixSerial::configurePort() {

    if(fd_ == -1) {
        return CROI_ERROR;
    }

    struct termios options;
    tcgetattr(fd_, &options);

    //Speed
    cfsetispeed(&options, settings_.baudrate);
    cfsetospeed(&options, settings_.baudrate);

    //c_flag
    options.c_cflag |= settings_.cflag;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    //save
    tcsetattr(fd_, TCSANOW, &options);

    return CROI_SUCCESS;
}

PosixSerial::~PosixSerial() {

    if(fd_ != -1) {
        close(fd_);
    }
}

int PosixSerial::initializePort()
{
    if(fd_ == -1) {
        if(openPort() == CROI_ERROR) {
            return CROI_ERROR;
        }
        if(configurePort() == CROI_ERROR) {
            return CROI_ERROR;
        }
    }
    initialized_ = true;
    return CROI_SUCCESS;
}

}
