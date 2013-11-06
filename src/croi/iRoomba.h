#ifndef IROOMBA_H
#define IROOMBA_H

#include <exception>
#include <string>
#include <QObject>
#include "poiQGraphicsEllipseItem.h"
#include <QGraphicsPolygonItem>
#include <QPointF>

namespace Croi {

class IRoomba: public QObject
{
    Q_OBJECT

public:

    IRoomba(QObject* parent = NULL);
    virtual ~IRoomba();

    virtual int rmb_connect(std::string ip) = 0;
    virtual int disconnect() = 0;
    virtual void safeMode() = 0;
    virtual void fullMode() = 0;
    virtual void allMotorsOn() = 0;
    virtual void allMotorsOff() = 0;
    virtual void clean() = 0;
    virtual void Drive( int Velocity, int Radius );
    virtual void playSong( int songNumber ) = 0;
    virtual char getTemperature() = 0;
    virtual unsigned short getChargeLevel() = 0;
    virtual short getDistance() = 0;
    virtual short getAngle() = 0;
    int getRadius();
    int getVelocity();
    //virtual bool isConnected = 0;
    void resetAngle();
    QPointF getRoombasLocation();
    double getCurrentAngle();
    void updateState(int distance, int angle, int radius, int speed);
    poiQGraphicsEllipseItem* getStartPoint();
    void setStartPoint(poiQGraphicsEllipseItem* startPoint);
    QGraphicsPolygonItem* getCurPoint();
    void setCurPoint(QGraphicsPolygonItem*);
    QGraphicsLineItem* getCurSpeed();
    void setCurSpeed(QGraphicsLineItem* curSpeed);
    QVector<QGraphicsLineItem*>* getTraces();
    void ifShowTraces();
    void removeTraces();

private:
    poiQGraphicsEllipseItem* startPoint_;
    QGraphicsPolygonItem* curPoint_;
    QGraphicsLineItem* curSpeed_;
    QVector<QGraphicsLineItem*> traces_;
    bool traceShown_;
    double initX_;
    double initY_;
    double angle_;
    int radius_;
    int velocity_;
};


} //namespace CROI


#endif // IROOMBA_H
