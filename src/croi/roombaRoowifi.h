#ifndef ROOMBAROOWIFI_H
#define ROOMBAROOWIFI_H

#include "iRoomba.h"
#include "../ext_lib/RooWifi_11b/roowifi.h"

namespace Croi {

class RoombaRoowifi : public IRoomba
{
    Q_OBJECT
public:
    RoombaRoowifi(PoiQGraphicsEllipseItem *startPoint, MapQGraphicsView* map,
                  FleetManager *parent = NULL);
    virtual ~RoombaRoowifi();

    virtual int rmb_connect(std::string ip);
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
    virtual bool getLeftBumb();
    virtual bool getRightBumb();

    virtual void goDock();
    virtual float getBatteryLevel();
public slots:
    void testSlot();
private:
    RooWifi* roowifi_;
};

} //namespace Croi
#endif // ROOMBAROOWIFI_H
