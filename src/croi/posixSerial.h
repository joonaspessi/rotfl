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

namespace CROI {

class PosixSerial {
public:

	PosixSerial(SerialSettings settings);

	PosixSerial();

    int writeSerial(const std::string buf);

    int readSerial(std::string & buf);

	virtual ~PosixSerial();

private:

    int initializePort();

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
