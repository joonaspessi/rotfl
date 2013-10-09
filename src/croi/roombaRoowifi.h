#ifndef ROOMBAROOWIFI_H
#define ROOMBAROOWIFI_H

#include "iRoomba.h"
#include "../ext_lib/RooWifi_11b/roowifi.h"

namespace Croi {

class RoombaRoowifi : public QObject, public IRoomba
{
    Q_OBJECT
public:
    RoombaRoowifi(QObject *parent);
    virtual ~RoombaRoowifi();

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

public slots:
    void testSlot();
private:
    RooWifi* roowifi_;
};

} //namespace Croi
#endif // ROOMBAROOWIFI_H
