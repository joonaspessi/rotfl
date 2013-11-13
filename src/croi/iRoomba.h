#ifndef IROOMBA_H
#define IROOMBA_H

#include <exception>
#include <string>
#include <QObject>
#include <QGraphicsPolygonItem>
#include <QPointF>
#include "poiQGraphicsEllipseItem.h"

const double PI = 3.14159265;
const double ANGLECORRECTION = 3.05;
const double DISTANCECORRECTION = 6.1;

class MapQGraphicsView;

namespace Croi {

class IRoomba: public QObject
{
    Q_OBJECT

public:

    explicit IRoomba(PoiQGraphicsEllipseItem *startPoint,
                     MapQGraphicsView* map, QObject* parent = NULL);
    virtual ~IRoomba();

    virtual int rmb_connect(std::string ip) = 0;
    //subclass calls this implementation for setting isReady_ = false
    virtual int disconnect();
    //subclass calls this implementation for setting isReady_ = true
    virtual void safeMode();
    //subclass calls this implementation for setting isReady_ = true
    virtual void fullMode();
    virtual void allMotorsOn() = 0;
    virtual void allMotorsOff() = 0;
    virtual void clean() = 0;
    //subclass calls this implementation for setting velocity_ and radius_
    virtual void drive( int velocity, int radius );
    //subclass calls this implementation for setting velocity_
    virtual void drive( int velocity);
    virtual void playSong( int songNumber ) = 0;
    virtual char getTemperature() = 0;
    virtual unsigned short getChargeLevel() = 0;
    virtual short getDistance() = 0;
    virtual short getAngle() = 0;

    //all functions below do not need reimplementation by subclasses
    int getRadius();
    int getVelocity();
    //virtual bool isConnected = 0;
    void resetAngle();
    QPointF getLoc();  //Xloc_ and Yloc_ is given as a point
    double getCurrentAngle();
    //this function updates the class variables to represent actual roomba.
    //It's called in regular intervals in timer timeouts.
    void updateState();
    PoiQGraphicsEllipseItem* getStartPoint();
    void setStartPoint(PoiQGraphicsEllipseItem* startPoint);
    //TODO: changed when implementing new roomba icon
    QGraphicsPolygonItem* getPolygon();
    //TODO: changed when implementing new roomba icon
    void setPolygon(QGraphicsPolygonItem*);
    QGraphicsLineItem* getCurSpeed();
    void setCurSpeed(QGraphicsLineItem* curSpeed);
    QVector<QGraphicsLineItem*>* getTraces();
    //calling this will show/unshow traces depending on whether
    //they'recurrently shown
    void ifShowTraces();
    void removeTraces();
    //is roomba ready to receive drive commands
    bool isReady();

private:
    PoiQGraphicsEllipseItem* startPoint_;
    MapQGraphicsView* map_;
    //roombas triangle is stored in this.
    //TODO: make better icon
    QGraphicsPolygonItem* polygon_;
    //roomba's speedvector
    QGraphicsLineItem* curSpeed_;
    //roombas traces are shown here
    QVector<QGraphicsLineItem*> traces_;
    bool traceShown_;
    //current location's x-component
    double Xloc_;
    //current location's y-component
    double Yloc_;
    //current angle.
    double angle_;
    int radius_;
    int velocity_;
    bool isReady_;
};


} //namespace CROI


#endif // IROOMBA_H
