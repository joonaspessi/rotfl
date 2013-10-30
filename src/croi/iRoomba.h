#ifndef IROOMBA_H
#define IROOMBA_H

#include <exception>
#include <string>

namespace Croi {

class IRoomba
{

public:
    virtual ~IRoomba() {}

    virtual int rmb_connect(std::string ip) = 0;
    virtual int disconnect() = 0;
    virtual void safeMode() = 0;
    virtual void fullMode() = 0;
    virtual void allMotorsOn() = 0;
    virtual void allMotorsOff() = 0;
    virtual void clean() = 0;
    virtual void Drive( int Velocity, int Radius ) = 0;
    virtual void playSong( int songNumber ) = 0;
    virtual char getTemperature() = 0;
    virtual unsigned short getChargeLevel() = 0;
    virtual short getDistance() = 0;
    virtual short getAngle() = 0;
    virtual int getRadius() = 0;
    virtual int getVelocity() = 0;
    //virtual bool isConnected = 0;


};


} //namespace CROI


#endif // IROOMBA_H
