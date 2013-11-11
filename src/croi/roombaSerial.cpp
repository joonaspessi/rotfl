#include <QDebug>
#include <iostream>
#include <QString>
#include <sstream>
#include <string>
#include "roombaSerial.h"
#include "posixSerial.h"
#include "croiUtil.h"


namespace Croi {

RoombaSerial::RoombaSerial(PoiQGraphicsEllipseItem *startPoint,
                           MapQGraphicsView* map, QObject *parent) :
    IRoomba(startPoint, map, parent)
{
    posixSerial_ = new PosixSerial();
}

RoombaSerial::~RoombaSerial()
{
    if (posixSerial_  != nullptr) {
        delete posixSerial_;
    }
}

int RoombaSerial::rmb_connect(std::string ip)
{

    int rval = posixSerial_->initializePort();

    if(rval == Croi::CROI_ERROR) {
        return Croi::CROI_ERROR;
    }
    qDebug() << "rmb_connect";

    char buf = START;
    posixSerial_->writeSerial(&buf,1);

    return Croi::CROI_SUCCESS;

}

int RoombaSerial::disconnect()
{
    IRoomba::disconnect();
}

void RoombaSerial::safeMode()
{
    char buf = SAFE;
    posixSerial_->writeSerial(&buf,1);
    IRoomba::safeMode();
}

void RoombaSerial::fullMode()
{
    IRoomba::fullMode();
}

void RoombaSerial::allMotorsOn()
{

}

void RoombaSerial::allMotorsOff()
{

}

void RoombaSerial::clean()
{
    char buf = CLEAN;
    posixSerial_->writeSerial(&buf,1);
    qDebug() << "clean";
//    posixSerial_->writeSerial(optCodeToStr(CLEAN));
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
    char command[2];

    command[0] = QUERY;
    command[1] = 2;

    posixSerial_->writeSerial(command,2);

    char recv[255];
    size_t retSize = 0;

    posixSerial_->readSerial(recv, retSize);


    int dist = ((short)recv[2] << 8 | (short)recv[3]);

    int angl = (recv[4] << 8 | recv[5]);

    std::cout << "paluuarvo " << retSize << "dist " << dist << "angl " << angl << std::endl;





}

//int RoombaSerial::getRadius()
//{

//}

//int RoombaSerial::getVelocity()
//{

//}

std::string RoombaSerial::optCodeToStr(int optCode)
{
    char cmd = static_cast<char>(optCode);
    std::stringstream stream;
    stream << std::hex << cmd;
    std::string command(stream.str());
    return command;

}

void RoombaSerial::drive(int velocity, int radius)
{
    char command[5];

    command[0] = DRIVE;
    command[1] = ( velocity >> 8 ) & 0x00FF;
    command[2] = velocity & 0x00FF;

    command[3] = ( radius >> 8 ) & 0x00FF;
    command[4] = radius & 0x00FF;

    if(posixSerial_->writeSerial(command, 5) != CROI_SUCCESS){
    }

    IRoomba::drive(velocity, radius);
}

void RoombaSerial::drive(int velocity)
{
    char command[5];

    command[0] = DRIVE;
    command[1] = ( velocity >> 8 ) & 0x00FF;
    command[2] = velocity & 0x00FF;

    command[3] = ( getRadius() >> 8 ) & 0x00FF;
    command[4] = getRadius() & 0x00FF;

    if(posixSerial_->writeSerial(command, 5) != CROI_SUCCESS){
    }

    IRoomba::drive(velocity);
}

void RoombaSerial::playSong(int songNumber)
{

}

} //namespace Croi
