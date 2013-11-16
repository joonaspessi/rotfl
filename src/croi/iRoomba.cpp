#include "iRoomba.h"
#include "mapQGraphicsView.h"
#include "fleetManager.h"
#include "math.h"

namespace Croi {

IRoomba::IRoomba(PoiQGraphicsEllipseItem *startPoint, MapQGraphicsView *map,
                 FleetManager* parent):
    QObject(parent),
    startPoint_(startPoint), map_(map), polygon_(NULL), icon_(NULL),
    curSpeed_(NULL), Xloc_(startPoint->x()), Yloc_(startPoint->y()), angle_(0.0),
    radius_(RADSTRAIGHT), velocity_(0), traceShown_(true), isReady_(false)
{
    //QPixmap pixmap;
    //pixmap.load(":/icons/roomba_small");
    //icon_(pixmap, map);

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
    angle_ = fmod(angle_,2.0*PI);  //full circles are taken out -> range 0..2*PI

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
        //opacity, so we get the idea which parts are cleaned well
        traceL->setOpacity(0.25);
        traces_.append(traceL);
        if (!traceShown_) //if traces are currently hidden
        {
            traceL->setVisible(false);
        }
        map_->scene()->addItem(traceL);
    }

    Xloc_ = x;
    Yloc_ = y;

    //normalized speed
    double speed = static_cast<double>(velocity_)/500.0;

    if(speed < 0)
    {
        speed *= -1;
    }
    //making the correctly angled roombaTriangle
    //THIS WILL BE REPLACED WITH ROOMBA ICON
    QVector<QPointF> points;
    QPointF first(Xloc_+cos(angle_)*ARROWWIDTH, Yloc_+sin(angle_)*ARROWWIDTH);
    points.append(first);
    double tempAngle = angle_+40.0*PI/180.0;
    points.append(QPointF(Xloc_-cos(tempAngle)*ARROWWIDTH,
                          Yloc_-sin(tempAngle)*ARROWWIDTH));
    tempAngle -= 80.0*PI/180.0;
    points.append(QPointF(Xloc_-cos(tempAngle)*ARROWWIDTH,
                          Yloc_-sin(tempAngle)*ARROWWIDTH));
    points.append(first);
    QPolygonF triangle(points);
    //calculate the point at the back of the triangle
    double triangleX = (points.at(1).x()+points.at(2).x())/2.0;
    double triangleY = (points.at(1).y()+points.at(2).y())/2.0;

    //ROOMBA'S ICON WILL REPLACE THIS IMPLEMENTATION
    if (polygon_ == NULL)  //first update
    {
        polygon_ = map_->scene()->addPolygon(triangle);
        //color of the roombaTriangle is blue
        QBrush triangleBrush(Qt::GlobalColor::blue);
        polygon_->setBrush(triangleBrush);
        polygon_->setFlag(QGraphicsItem::ItemIsSelectable, true);
        polygon_->setFlag(QGraphicsItem::ItemIsMovable,false);
        QPen curSpeedPen(Qt::GlobalColor::blue);
        curSpeedPen.setWidth(TRACEWIDTH/4.0);
        //don't you dare say that this here piece of code is not beautiful!
        curSpeed_ = map_->scene()->addLine(triangleX, triangleY,
                                     triangleX-cos(angle_)*ARROWWIDTH*2.0*speed,
                                     triangleY-sin(angle_)*ARROWWIDTH*2.0*speed,
                                     curSpeedPen);
        curSpeed_->setFlag(QGraphicsItem::ItemIsMovable,false);
    }
    else
    {
        polygon_->setPolygon(triangle);
        curSpeed_->setLine(triangleX, triangleY,
                          triangleX-cos(angle)*ARROWWIDTH*2.0*speed,
                          triangleY-sin(angle)*ARROWWIDTH*2.0*speed);
    }
    curSpeed_->setZValue(1);
    polygon_->setZValue(1);

   qobject_cast<FleetManager*>(parent())->checkPoiCollision();
}

QGraphicsPolygonItem* IRoomba::getPolygon()
{
    return polygon_;
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
