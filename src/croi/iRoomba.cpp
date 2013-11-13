#include "iRoomba.h"
#include "mapQGraphicsView.h"
#include "fleetManager.h"

namespace Croi {

IRoomba::IRoomba(PoiQGraphicsEllipseItem *startPoint, MapQGraphicsView *map, QObject *parent):
    QObject(parent),
    startPoint_(startPoint), map_(map), polygon_(NULL), curSpeed_(NULL),
    Xloc_(startPoint->x()), Yloc_(startPoint->y()), angle_(0.0),
    radius_(RADSTRAIGHT), velocity_(0), traceShown_(true), isReady_(false)
{

}

int IRoomba::disconnect()
{
    isReady_ = false;
}

void IRoomba::safeMode()
{
    isReady_ = true;
}

void IRoomba::fullMode()
{
    isReady_ = true;
}

void IRoomba::drive( int velocity, int radius )
{
    radius_ = radius;
    velocity_ = velocity;
}

//this overloaded function does not change radius
void IRoomba::drive( int velocity)
{
    velocity_ = velocity;
}

PoiQGraphicsEllipseItem* IRoomba::getStartPoint()
{
    return startPoint_;
}

void IRoomba::setStartPoint(PoiQGraphicsEllipseItem* startPoint)
{
    startPoint_= startPoint;
}

void IRoomba::resetAngle()
{
    angle_ = 0.0;
}

QPointF IRoomba::getLoc()
{
    QPointF point(Xloc_, Yloc_);
    return point;

}

double IRoomba::getCurrentAngle()
{
    return angle_;
}

QVector<QGraphicsLineItem*>* IRoomba::getTraces()
{
    return &traces_;
}

int IRoomba::getRadius()
{
    return radius_;
}

int IRoomba::getVelocity()
{
    return velocity_;
}

void IRoomba::updateState()
{
    //subclass handles the retrieval of sensor information
    int distance = getDistance();
    int angle = getAngle();

    //angle for distance calculation
    double angleForDist = angle_-static_cast<double>(angle)*PI*ANGLECORRECTION/180.0;
    //distance changed to cm
    double dist = -static_cast<double>(distance)/10.0*DISTANCECORRECTION;
    //special radiuses mean no adaptation needed
    if (radius_ != 32768 && radius_ != 32767 && radius_ != -1 && radius_ != 1)
    {
        //corrected distance (and change to cm)
        dist = -2.0*(static_cast<double>(radius_))*
                sin(static_cast<double>(distance)/radius_/2)/10.0*DISTANCECORRECTION;
        //corrected angle in radians for distance calculation
        //angleForDist = static_cast<double>(angle)*PI/180.0*ANGLECORRECTION/2.0;
        //other version that doesn't work curently
        angleForDist = angle_-static_cast<double>(distance)/radius_/2.0;
    }
    //real angle (always used for roomba's angle)
    angle_ -= static_cast<double>(angle)*PI*ANGLECORRECTION/180.0;

    //coordinates are updated
    double x = Xloc_+cos(angleForDist)*dist;
    double y = Yloc_+sin(angleForDist)*dist;

    if (Xloc_ != x || Yloc_ != y)
    {
        //new piece of trace is created (polygon_ set as parent)
        QGraphicsLineItem* traceL = new QGraphicsLineItem
                (Xloc_, Yloc_, x, y);
        QPen linePen(Qt::GlobalColor::gray);
        linePen.setWidth(TRACEWIDTH);
        traceL->setPen(linePen);
        traces_.append(traceL);
        if (!traceShown_) //if traces are currently hidden
        {
            traceL->setVisible(false);
        }
        map_->scene()->addItem(traceL);
    }
    Xloc_ = x;
    Yloc_ = y;
}

QGraphicsPolygonItem* IRoomba::getPolygon()
{
    return polygon_;
}

void IRoomba::setPolygon(QGraphicsPolygonItem* polygon)
{
    polygon_ = polygon;
    polygon_->setFlag(QGraphicsItem::ItemIsSelectable, true);
    polygon_->setFlag(QGraphicsItem::ItemIsMovable,false);
}

QGraphicsLineItem* IRoomba::getCurSpeed()
{
    return curSpeed_;
}

void IRoomba::setCurSpeed(QGraphicsLineItem* curSpeed)
{
    curSpeed_ = curSpeed;
}

void IRoomba::ifShowTraces()
{
    if (traceShown_)
    {
        traceShown_ = false;

        for (QVector<QGraphicsLineItem*>::iterator i = traces_.begin();
            i != traces_.end(); ++i)
        {
            (*i)->setVisible(false);
        }
    }
    else
    {
        traceShown_ = true;

        for (QVector<QGraphicsLineItem*>::iterator i = traces_.begin();
            i != traces_.end(); ++i)
        {
            (*i)->setVisible(true);
        }
    }
}

void IRoomba::removeTraces()
{
    for (QVector<QGraphicsLineItem*>::iterator i = traces_.begin();
        i != traces_.end(); ++i)
    {
        map_->scene()->removeItem(*i);
        delete *i;
    }
    traces_.clear();
}

bool IRoomba::isReady()
{
    return isReady_;
}

void IRoomba::sensorUpdateTimerTimeout()
{
    updateState();
}

IRoomba::~IRoomba()
{

}

}
