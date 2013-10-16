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

int RoombaSerial::rmb_connect()
{
    int rval = posixSerial_->initializePort();

    if(rval == Croi::CROI_ERROR) {
        return Croi::CROI_ERROR;
    }
    return Croi::CROI_SUCCESS;

}

int RoombaSerial::disconnect()
{

}

void RoombaSerial::safeMode()
{

}

void RoombaSerial::fullMode()
{

}

void RoombaSerial::allMotorsOn()
{

}

void RoombaSerial::allMotorsOff()
{

}

void RoombaSerial::clean()
{

}

char RoombaSerial::getTemperature()
{

}

unsigned short RoombaSerial::getChargeLevel()
{

}

short RoombaSerial::getDistance()
{

}

short RoombaSerial::getAngle()
{

}

void RoombaSerial::Drive(int Velocity, int Radius)
{
    if(posixSerial_->writeSerial("128+Velocity+Radius") != CROI_SUCCESS){
    }
}

void RoombaSerial::playSong(int songNumber)
{

}

} //namespace Croi
