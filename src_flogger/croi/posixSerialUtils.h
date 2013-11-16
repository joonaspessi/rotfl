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

namespace Croi {

struct SerialSettings {
	speed_t baudrate; //eg. B19200
	tcflag_t cflag;

};


const std::string DEFAULT_DEVICE = "/dev/ttyUSB0";

const unsigned int MAX_READ = 255;

} //namespace Croi


#endif /* SERIALSETTINGS_H_ */
