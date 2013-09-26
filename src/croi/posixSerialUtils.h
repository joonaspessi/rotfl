/*
 * serialSettings.h
 *
 *  Created on: Sep 20, 2013
 *      Author: pessi
 */

#ifndef SERIALSETTINGS_H_
#define SERIALSETTINGS_H_

extern "C" {
#include <termios.h>
}

namespace CROI {

struct SerialSettings {
	speed_t baudrate; //eg. B19200
	tcflag_t cflag;

};

enum returnval {
	CROI_ERROR = -1,
	CROI_SUCCESS = 0
};

const std::string DEFAULT_DEVICE = "/dev/ttyUSB0";

const unsigned int MAX_READ = 255;
}


#endif /* SERIALSETTINGS_H_ */
