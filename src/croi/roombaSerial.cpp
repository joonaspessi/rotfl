#include <exception>
#include "roombaSerial.h"
#include "posixSerial.h"
#include "croiUtil.h"


namespace Croi {

RoombaSerial::RoombaSerial() :
    posixSerial_(new PosixSerial())
{
}

RoombaSerial::~RoombaSerial()
{
    if (posixSerial_  != nullptr) {
        delete posixSerial_;
    }
}

int RoombaSerial::connect()
{
    int rval = posixSerial_->initializePort();

    if(rval == Croi::CROI_ERROR) {
        return Croi::CROI_ERROR;
    }
    return Croi::CROI_SUCCESS;

}

void RoombaSerial::Drive(int Velocity, int Radius)
{
    if(posixSerial_->writeSerial("128+Velocity+Radius") != CROI_SUCCESS){
    }
}

} //namespace Croi
