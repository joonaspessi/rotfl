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

    virtual int connect();
    virtual void Drive( int Velocity, int Radius );

private:
    PosixSerial* posixSerial_;
};

} //namespace Croi

#endif // ROOMBADRIVER_H
