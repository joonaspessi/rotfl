#include "iRoomba.h"
#include "mapQGraphicsView.h"
#include "fleetManager.h"
#include "math.h"
#include "QDebug"
#include <QObject>

namespace Croi {

IRoomba::IRoomba(PoiQGraphicsEllipseItem *startPoint, MapQGraphicsView *map,
                 FleetManager* parent):
    QObject(parent),
    startPoint_(startPoint), map_(map), polygon_(NULL), icon_(NULL),
    curSpeed_(NULL), Xloc_(startPoint->x()), Yloc_(startPoint->y()), angle_(0.0),
    radius_(RADSTRAIGHT), velocity_(0), traceShown_(true), isReady_(false), driveTime_(0)
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
    bool leftBump = getLeftBumb();
    bool rightBump = getRightBumb();

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
    //qDebug() << "Roomba's angle in degrees: " << angle_*(180/PI);
    angle_ = fmod(angle_, 2.0*PI);
    if (angle_ < 0)
    {
        angle_ = 2*PI+angle_;
    }
    //qDebug() << "Roomba's angle in degrees after correction: " << angle_*(180/PI);

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
    /*QVector<QPointF> points;
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
    */

    //ROOMBA'S ICON WILL REPLACE THIS IMPLEMENTATION
    if (icon_ == NULL)  //first update
    {
        QPixmap pixmap(":/icons/roomba_small");
        //pixmap.load(":/icons/roomba_small");
        //QSize size(34,34);
        //pixmap.scaled(size);
        icon_ = map_->scene()->addPixmap(pixmap);
        icon_->setOffset(-17, -17);
        icon_->setPos(Xloc_, Yloc_);
        icon_->setRotation(90);
        icon_->setFlag(QGraphicsItem::ItemIsSelectable, true);
        icon_->setFlag(QGraphicsItem::ItemIsMovable,false);
        //color of the roombaTriangle is blue
        /*QBrush triangleBrush(Qt::GlobalColor::blue);
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
        curSpeed_->setFlag(QGraphicsItem::ItemIsMovable,false);*/
    }
    else
    {
        /*
        polygon_->setPolygon(triangle);
        curSpeed_->setLine(triangleX, triangleY,
                          triangleX-cos(angle)*ARROWWIDTH*2.0*speed,
                          triangleY-sin(angle)*ARROWWIDTH*2.0*speed);
        */
        icon_->setPos(Xloc_, Yloc_);
        icon_->resetTransform();
        icon_->setRotation(angle_*(180/PI));

    }
    icon_->setZValue(1);
    //curSpeed_->setZValue(1);
    //polygon_->setZValue(1);

    //qobject_cast<FleetManager*>(parent())->checkPoiCollision();
    //add new wall if bumb has happened
    if (leftBump || rightBump)
    {
        qDebug() << "Roomba has collided with unknown object!";
        double tempAngle = angle_-40.0*PI/180.0;
        QPointF l (Xloc_+cos(tempAngle)*17, Yloc_+sin(tempAngle)*17);
        tempAngle = angle_+40.0*PI/180.0;
        QPointF r (Xloc_+cos(tempAngle)*17, Yloc_+sin(tempAngle)*17);
        WallQGraphicsLineItem* bumbed = new WallQGraphicsLineItem
                                            (l.x(), l.y(), r.x(), r.y());
        map_->scene()->addItem(bumbed);
    }

}

QGraphicsPixmapItem *IRoomba::getIcon()
{
    return icon_;
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

void IRoomba::go2Point(QPointF point)
{

//    qDebug() << "POI coordinate x: " << poiCoordinate.x()
//             << " , y: " << poiCoordinate.y();
//    qDebug() << "Roomba coordinate x: " << roombaCoordinate.x()
//             << " , y: " << roombaCoordinate.y();

    //calculate the angle

    float deltaX = point.x()-Xloc_;
    float deltaY = Yloc_ - point.y();
    //float angleRadian = atan2(deltaY, deltaX);
    //float anglePi = angleRadian*180 / PI;

//    qDebug() << "Delta X: " << deltaX;
//    qDebug() << "Delta Y: " << deltaY;

    float calculatedAngle = 0;

    if (deltaX == 0 && deltaY == 0)
    {
//        qDebug() << "Roomba is at POI";
        return;
    }
    else
    {
        calculatedAngle = -atan2(deltaY, deltaX);
    }

//    qDebug() << "Calculated angle in degrees: " << calculatedAngle*(180/PI);

    float turningAngle = 0.0;
    float roombasAngle = angle_; //0 - 2PI

    if (roombasAngle > PI)
    {
        roombasAngle -= 2*PI;
    }
//    qDebug() << "Roombas angle in degrees: " << roombasAngle*(180/PI);

    //both roombasAngle and calculatedAngle are between -PI and PI
    if (roombasAngle >= 0)
    {
        if (calculatedAngle >= 0)
        {
            if (calculatedAngle > roombasAngle)
            {
                turningAngle = calculatedAngle - roombasAngle;
            }
            else
            {
                turningAngle = calculatedAngle - roombasAngle;
            }
        }
        else //calculatedAngle < 0
        {
            if (calculatedAngle -roombasAngle < (-1*PI) )
            {
                turningAngle = calculatedAngle - roombasAngle + 2*PI;
            }
            else
            {
                turningAngle = calculatedAngle - roombasAngle;
            }
        }
    }
    else //roombasAngle < 0
    {
        if (calculatedAngle >= 0)
        {
            if ( calculatedAngle - roombasAngle > PI )
            {
                turningAngle = calculatedAngle - roombasAngle - 2*PI;
            }
            else
            {
                turningAngle = calculatedAngle - roombasAngle;
            }
        }
        else //calculatedAngle <0
        {
            if ( calculatedAngle > roombasAngle )
            {
                turningAngle = calculatedAngle - roombasAngle;
            }
            else
            {
                turningAngle = calculatedAngle - roombasAngle;
            }
        }
    }

    // Roomba turns 1 degree in 18055 microseconds, when speed is 100
    float tabs = abs(turningAngle*(180/PI));
    float distance = sqrt(pow(deltaX,2)+pow(deltaY,2) );
    int turnTime = tabs * 18055 / 1000;
    driveTime_= distance*100;

    qDebug() << "turningAngle: " << tabs << "ttime: " << turnTime;
    qDebug() << "distance: " << distance << "dtime: " << driveTime_;
    //QCoreApplication::processEvents(QEventLoop::AllEvents, 1);


//    qDebug() << "Turning angle in degrees: " << turningAngle*(180/PI);

    if (turningAngle > 0) //Turn clockwise
    {
        this->drive(100,RADTURNCW);

    }
    else
    {
        this->drive(100,RADTURNCCW);
    }
    QTimer::singleShot(turnTime, this, SLOT(turnTimerTimeout()));


}

void IRoomba::sensorUpdateTimerTimeout()
{
    updateState();
}

void IRoomba::turnTimerTimeout()
{
    this->drive(0,32767);
    this->drive(100, RADSTRAIGHT);
    QTimer::singleShot(driveTime_, this, SLOT(driveTimerTimeout()));

}

void IRoomba::driveTimerTimeout()
{
    this->drive(0,32767);

}

IRoomba::~IRoomba()
{

}

}
