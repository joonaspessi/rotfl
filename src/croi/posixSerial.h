/*
 * PosixSerial.h
 *
 *  Created on: Sep 20, 2013
 *      Author: pessi
 */

#ifndef POSIXSERIAL_H_
#define POSIXSERIAL_H_

#include <string>
#include "posixSerialUtils.h"

namespace Croi {

class PosixSerial  {
public:

    PosixSerial(SerialSettings settings);

    PosixSerial(std::string device);

    PosixSerial();

    int writeSerial(const std::string buf);

    int readSerial(std::string & buf);

    int initializePort();

    virtual ~PosixSerial();

private:

	int openPort();

	int configurePort();

	int fd_;
	bool initialized_;
	std::string device_;
	SerialSettings settings_;
    char readBuf_[MAX_READ];
};

}

#endif /* POSIXSERIAL_H_ */
