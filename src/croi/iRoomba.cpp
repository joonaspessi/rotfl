#include "iRoomba.h"
#include "mapQGraphicsView.h"

namespace Croi {

IRoomba::IRoomba(QObject *parent):
    QObject(parent),
    startPoint_(NULL), curPoint_(NULL), curSpeed_(NULL),
    initX_(0.0), initY_(0.0), angle_(0.0),
    radius_(1), velocity_(0), traceShown_(true)
{

}

void IRoomba::Drive( int Velocity, int Radius )
{
    radius_ = Radius;
    velocity_ = Velocity;
}

poiQGraphicsEllipseItem* IRoomba::getStartPoint()
{
    return startPoint_;
}

void IRoomba::setStartPoint(poiQGraphicsEllipseItem* startPoint)
{
    startPoint_= startPoint;
}

void IRoomba::resetAngle()
{
    angle_ = 0.0;
}

QPointF IRoomba::getRoombasLocation()
{
    QPointF point(initX_, initY_);
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

void IRoomba::updateState(int distance, int angle, int radius, int speed)
{
    //angle for distance calculation
    double angleForDist = angle_-static_cast<double>(angle)*PI*ANGLECORRECTION/180.0;
    //distance changed to cm
    double dist = -static_cast<double>(distance)/10.0*DISTANCECORRECTION;
    //special radiuses mean no adaptation needed
    if (radius != 32768 && radius != 32767 && radius != -1 && radius != 1)
    {
        //corrected distance (and change to cm)
        dist = -2.0*(static_cast<double>(radius))*
                sin(static_cast<double>(distance)/radius/2)/10.0*DISTANCECORRECTION;
        //corrected angle in radians for distance calculation
        //angleForDist = static_cast<double>(angle)*PI/180.0*ANGLECORRECTION/2.0;
        //other version that doesn't work curently
        angleForDist = angle_-static_cast<double>(distance)/radius/2.0;
    }
    //real angle (always used for roomba's angle)
    angle_ -= static_cast<double>(angle)*PI*ANGLECORRECTION/180.0;

    //coordinates are updated
    double x = initX_+cos(angleForDist)*dist;
    double y = initY_+sin(angleForDist)*dist;

    QGraphicsLineItem* traceL = new QGraphicsLineItem
            (initX_, initY_, x, y);
    QPen linePen(Qt::GlobalColor::gray);
    linePen.setWidth(TRACEWIDTH);
    traceL->setPen(linePen);
    traces_.append(traceL);

    initX_ = x;
    initY_ = y;
}

QGraphicsPolygonItem* IRoomba::getCurPoint()
{
    return curPoint_;
}

void IRoomba::setCurPoint(QGraphicsPolygonItem* curPoint)
{
    curPoint_ = curPoint;
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
        delete *i;
    }
    traces_.clear();
}

IRoomba::~IRoomba()
{

}

}
