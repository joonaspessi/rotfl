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
    virtual int disconnect();
    virtual void safeMode();
    virtual void fullMode();
    virtual void allMotorsOn() = 0;
    virtual void allMotorsOff() = 0;
    virtual void clean() = 0;
    virtual void drive( int velocity, int radius );
    virtual void drive( int velocity);
    virtual void playSong( int songNumber ) = 0;
    virtual char getTemperature() = 0;
    virtual unsigned short getChargeLevel() = 0;
    virtual short getDistance() = 0;
    virtual short getAngle() = 0;
    int getRadius();
    int getVelocity();
    //virtual bool isConnected = 0;
    void resetAngle();
    QPointF getLoc();
    double getCurrentAngle();
    void updateState();
    PoiQGraphicsEllipseItem* getStartPoint();
    void setStartPoint(PoiQGraphicsEllipseItem* startPoint);
    QGraphicsPolygonItem* getPolygon();
    void setPolygon(QGraphicsPolygonItem*);
    QGraphicsLineItem* getCurSpeed();
    void setCurSpeed(QGraphicsLineItem* curSpeed);
    QVector<QGraphicsLineItem*>* getTraces();
    void ifShowTraces();
    void removeTraces();
    bool isReady();

private:
    PoiQGraphicsEllipseItem* startPoint_;
    MapQGraphicsView* map_;
    QGraphicsPolygonItem* polygon_;
    QGraphicsLineItem* curSpeed_;
    QVector<QGraphicsLineItem*> traces_;
    bool traceShown_;
    double Xloc_;
    double Yloc_;
    double angle_;
    int radius_;
    int velocity_;
    bool isReady_;
};


} //namespace CROI


#endif // IROOMBA_H
