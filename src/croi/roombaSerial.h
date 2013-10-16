#ifndef ROOMBADRIVER_H
#define ROOMBADRIVER_H

#include "iRoomba.h"

namespace Croi {

class PosixSerial;

class RoombaSerial : public IRoomba
{

public:
    RoombaSerial();
    virtual ~RoombaSerial();

    virtual int rmb_connect();
    virtual int disconnect();
    virtual void safeMode();
    virtual void fullMode();
    virtual void allMotorsOn();
    virtual void allMotorsOff();
    virtual void clean();
    virtual void Drive( int Velocity, int Radius );
    virtual void playSong( int songNumber );
    virtual char getTemperature();
    virtual unsigned short getChargeLevel();
    virtual short getDistance();
    virtual short getAngle();

private:
    PosixSerial* posixSerial_;
};

} //namespace Croi

#endif // ROOMBADRIVER_H
