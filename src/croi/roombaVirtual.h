#ifndef ROOMBAVIRTUAL_H
#define ROOMBAVIRTUAL_H

#include "iRoomba.h"
#include <QTimer>
#include <QGraphicsPixmapItem>

namespace Croi {

class RoombaVirtual : public IRoomba
{
    Q_OBJECT

public:
    RoombaVirtual(PoiQGraphicsEllipseItem *startPoint, MapQGraphicsView* map,
                  FleetManager *parent = NULL);
    virtual ~RoombaVirtual();

    virtual int rmb_connect(std::string ip);
    virtual int disconnect();
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
    virtual int getDirtAmount();
    //sets a virtual roomba icon
    virtual QGraphicsPixmapItem* setIcon();
    virtual void goDock();
    virtual float getBatteryLevel();

signals:

public slots:
    void statusTimerTimeout();

private:
    QTimer* statusTimer_;
    double deltaDistance_;
    double deltaAngle_;
    int lastDriveCmdTime_;
};

} //namespace Croi
#endif // ROOMBAVIRTUAL_H
