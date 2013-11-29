#include "iRoomba.h"
#include "mapQGraphicsView.h"
#include "fleetManager.h"
#include "math.h"
#include "QDebug"
#include <QObject>
#include <queue>
#include <vector>
#include <cmath>
#include <limits>

namespace Croi {

IRoomba::IRoomba(PoiQGraphicsEllipseItem *startPoint, MapQGraphicsView *map,
                 FleetManager* parent):
    QObject(parent),
    startPoint_(startPoint), map_(map), polygon_(NULL), icon_(NULL),
    curSpeed_(NULL), Xloc_(startPoint->x()), Yloc_(startPoint->y()), angle_(0.0),
    radius_(RADSTRAIGHT), velocity_(0), traceShown_(true), isReady_(false), driveTime_(0),
    path_(NULL)
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

    if (icon_ == NULL)  //first update
    {
         // TODO: Improve icon graphics before deploying
        QPixmap pixmap(":/icons/DEBUG_roomba_small");
        icon_ = map_->scene()->addPixmap(pixmap);
        icon_->setOffset(-17, -17);
        icon_->setPos(Xloc_, Yloc_);
        icon_->setFlag(QGraphicsItem::ItemIsSelectable, true);
        icon_->setFlag(QGraphicsItem::ItemIsMovable,false);
    }
    else
    {
        icon_->setPos(Xloc_, Yloc_);
        icon_->resetTransform();
        // Roomba icon is in angle of 270 by default in the pixmap, setRotation accepts also negative angles
        icon_->setRotation((angle_*(180/PI))-270);
    }
    icon_->setZValue(1);

    qobject_cast<FleetManager*>(parent())->checkPoiCollision();

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

double IRoomba::calcPath(QVector<QVector<Util::Vertice*>> vertices, QPointF point)
{
    //find the start vertice
    Util::Vertice* startV = NULL;
    Util::Vertice* curV = NULL;
    Util::Vertice* goalV = NULL;
    for(unsigned int i = 1; i < vertices.size(); ++i)
    {
        for(unsigned int j = 1; j < vertices.size(); ++j)
        {
            if(Xloc_ >= vertices.at(i).at(j)->topLeftX &&
               Xloc_ <= vertices.at(i).at(j)->topLeftX+Util::VERTICEWIDTH-1 &&
               Yloc_ >= vertices.at(i).at(j)->topLeftY &&
               Yloc_ <= vertices.at(i).at(j)->topLeftY+Util::VERTICEWIDTH-1)
            {
                startV = vertices.at(i).at(j);
            }
            if(point.x() >= vertices.at(i).at(j)->topLeftX &&
               point.x() <= vertices.at(i).at(j)->topLeftX+Util::VERTICEWIDTH-1 &&
               point.y() >= vertices.at(i).at(j)->topLeftY &&
               point.y() <= vertices.at(i).at(j)->topLeftY+Util::VERTICEWIDTH-1)
            {
                goalV = vertices.at(i).at(j);
            }
        }
        if(startV != NULL && goalV != NULL) //just a bit of speed optimizing
        {
            break;
        }
    }

    std::priority_queue<Util::Vertice*, std::vector<Util::Vertice*>,
                        bool (*)(Util::Vertice*, Util::Vertice*)>
            priQ(&IRoomba::verticeCompare);

    //Dijkstra's algorithm
    startV->dist = 0.0;
    priQ.push(startV);
    while(!priQ.empty())
    {
        curV = priQ.top();
        priQ.pop();
        if(curV == goalV)
        {
            break;  //we got what we came for
        }
        compNeigh(curV, Util::N, &priQ);
        compNeigh(curV, Util::NE, &priQ);
        compNeigh(curV, Util::E, &priQ);
        compNeigh(curV, Util::SE, &priQ);
        compNeigh(curV, Util::S, &priQ);
        compNeigh(curV, Util::SW, &priQ);
        compNeigh(curV, Util::W, &priQ);
        compNeigh(curV, Util::NW, &priQ);
    }

    path_ = new QStack<QPointF>;
    while(curV != startV)
    {
        path_->push(curV->pos);
        curV = curV->from;
    }

    double dist = goalV->dist;

    //resetting vertice info
    for(unsigned int i = 1; i < vertices.size(); ++i)
    {
        for(unsigned int j = 1; j < vertices.size(); ++j)
        {
            vertices.at(i).at(j)->dist = std::numeric_limits<double>::max();
            vertices.at(i).at(j)->from = NULL;
        }
    }

    return dist;
}

void IRoomba::compNeigh(Util::Vertice* curV, Util::Direction direction,
                        std::priority_queue<Util::Vertice *,
                                            std::vector<Util::Vertice*>,
                                            bool (*)(Util::Vertice*, Util::Vertice*)>* priQ)
{
    double dist = 0.0;  //distance between curV and neighV
    Util::Vertice* neighV = NULL;

    switch(direction)
    {
    case Util::N:
        dist = 1.0;
        neighV = curV->n;
        break;
    case Util::NE:
        dist = sqrt(2.0);
        neighV = curV->ne;
        break;
    case Util::E:
        dist = 1.0;
        neighV = curV->e;
        break;
    case Util::SE:
        dist = sqrt(2.0);
        neighV = curV->se;
        break;
    case Util::S:
        dist = 1.0;
        neighV = curV->s;
        break;
    case Util::SW:
        dist = sqrt(2.0);
        neighV = curV->sw;
        break;
    case Util::W:
        dist = 1.0;
        neighV = curV->w;
        break;
    case Util::NW:
        dist = sqrt(2.0);
        neighV = curV->nw;
        break;
    }

    if(neighV == NULL) //no link to neighbour -> nothing is done
    {
        return;
    }

    if(neighV->dist == std::numeric_limits<double>::max())
    {
        priQ->push(neighV);
    }

    //relax
    if(neighV->dist > curV->dist+dist)
    {
        neighV->dist = curV->dist+dist;
        neighV->from = curV;
    }
}

//function for comparing vertices
bool IRoomba::verticeCompare(Util::Vertice* first, Util::Vertice* second)
{
    return first->dist < second->dist;
}

IRoomba::~IRoomba()
{

}

}
