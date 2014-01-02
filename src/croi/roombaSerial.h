#ifndef ROOMBADRIVER_H
#define ROOMBADRIVER_H

#include "iRoomba.h"

namespace Croi {

class PosixSerial;

class RoombaSerial : public IRoomba
{

public:
    RoombaSerial(PoiQGraphicsEllipseItem *startPoint, MapQGraphicsView* map,
                 FleetManager *parent);
    virtual ~RoombaSerial();

    virtual int rmb_connect(std::string  ip);
    virtual int disconnect();
    virtual void safeMode();
    virtual void fullMode();
    virtual void allMotorsOn();
    virtual void allMotorsOff();
    virtual void clean();
    virtual void drive( int velocity, int radius );
    virtual void drive( int velocity);
    virtual void playSong( int songNumber );
    virtual char getTemperature();
    virtual unsigned short getChargeLevel();
    virtual double getDistance();
    virtual double getAngle();
    //virtual int getRadius();
    //virtual int getVelocity();

private:
    std::string optCodeToStr(int optCode);
    PosixSerial* posixSerial_;
};

} //namespace Croi

#endif // ROOMBADRIVER_H
