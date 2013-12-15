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
    followingPath_(false), prevPReached_(false)
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
        //qobject_cast<FleetManager*>(parent())->addWall(bumbed);
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
    float tabs = abs(turningAngle*(180.0/PI));
    float distance = sqrt(pow(deltaX,2.0)+pow(deltaY,2.0));
    int turnTime = round(tabs * 18055.0/1000.0);
    driveTime_= round(distance*100.0);

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

    //nothing needs to be done atm if either one of these is false
    if(!followingPath_ || !prevPReached_)
    {
        return;
    }

    if(path_.isEmpty())
    {
        prevPReached_ = false;
        followingPath_ = false;

        for(unsigned int i = 0; i < pathLines_.size(); ++i)
        {
            map_->scene()->removeItem(pathLines_.at(i));
            delete pathLines_.at(i);
        }
        pathLines_.clear();
    }
    else
    {
        prevPReached_ = false;
        go2Point(path_.pop());
    }
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
    prevPReached_ = true;
}

double IRoomba::calcPath(QVector<QVector<Util::Vertice *> > &vertices, QPointF& point)
{
//commented lines skip the shortest path to perform simple go2point
//    path_.push(point);
//    return 1;
    //find the start vertice
    Util::Vertice* startV = NULL;
    Util::Vertice* curV = NULL;
    Util::Vertice* goalV = NULL;
    for(unsigned int i = 0; i < vertices.size(); ++i)
    {
        for(unsigned int j = 0; j < vertices.at(i).size(); ++j)
        {
//            qDebug() << "verticeX: " << vertices.at(i).at(j)->topLeftX << " "
//                     << "verticeY: " << vertices.at(i).at(j)->topLeftY;
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
        compNeigh(curV, Util::N, priQ);
        compNeigh(curV, Util::NE, priQ);
        compNeigh(curV, Util::E, priQ);
        compNeigh(curV, Util::SE, priQ);
        compNeigh(curV, Util::S, priQ);
        compNeigh(curV, Util::SW, priQ);
        compNeigh(curV, Util::W, priQ);
        compNeigh(curV, Util::NW, priQ);
    }

    //the points are pushed to path_. IRoomba doesn't first go to the middle
    //of its startV and it also skips the center of the goalV
    unsigned int straightCount = 0;  //unnecessary path points will be taken away
    path_.push(point);  //we want to go exactly to the end point
    if(curV->from != NULL)  //for drawing line exactly to the goal point
    {
        pathLines_.append(new QGraphicsLineItem(point.x(), point.y(), curV->from->pos.x(), curV->from->pos.y()));
    }

    if(curV != startV) //possible that curV is startV in the beginning
    {
        while(curV->from != startV)
        {
            curV = curV->from;
            //center point of vertice isn't pushed to path_ if is a part of a straight line.
            //maximum of ceil(1.0/Util::VERTICEWIDTH*50) vertices can be ignored from a path.
            //The limit is there to avoid problems with the slight inaccuracy in going to a point
            if(curV->from != startV &&
               straightCount <= static_cast<unsigned int>(ceil(1.0/Util::VERTICEWIDTH*50.0)) &&
               ((curV->from == curV->n && curV->from->from == curV->from->n) ||
               (curV->from == curV->ne && curV->from->from == curV->from->ne) ||
               (curV->from == curV->e && curV->from->from == curV->from->e) ||
               (curV->from == curV->se && curV->from->from == curV->from->se) ||
               (curV->from == curV->s && curV->from->from == curV->from->s) ||
               (curV->from == curV->sw && curV->from->from == curV->from->sw) ||
               (curV->from == curV->w && curV->from->from == curV->from->w) ||
               (curV->from == curV->nw && curV->from->from == curV->from->nw)))
            {
                if(straightCount == 0)
                {
                    pathLines_.append(new QGraphicsLineItem(path_.top().x(), path_.top().y(), curV->pos.x(), curV->pos.y()));
                    path_.push(curV->pos);
                }
                ++straightCount;
            }
            else
            {
                straightCount = 0;
                pathLines_.append(new QGraphicsLineItem(path_.top().x(), path_.top().y(), curV->pos.x(), curV->pos.y()));
                path_.push(curV->pos);
            }
        }
    }

    double dist = goalV->dist;

    //resetting vertice info
    for(unsigned int i = 0; i < vertices.size(); ++i)
    {
        for(unsigned int j = 0; j < vertices.size(); ++j)
        {
            vertices.at(i).at(j)->dist = std::numeric_limits<double>::max();
            vertices.at(i).at(j)->from = NULL;
        }
    }

    return dist;
}

void IRoomba::compNeigh(Util::Vertice *curV, Util::Direction direction,
                        std::priority_queue<Util::Vertice *,
                                            std::vector<Util::Vertice *>,
                                            bool (*)(Util::Vertice *, Util::Vertice *)> &priQ)
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
        priQ.push(neighV);
    }

    //relax
    if(neighV->dist > curV->dist+dist)
    {
        neighV->dist = curV->dist+dist;
        neighV->from = curV;
    }
}

void IRoomba::usePath()
{
    for(unsigned int i = 0; i < pathLines_.size(); ++i)
    {
        map_->scene()->addItem(pathLines_.at(i));
    }


    followingPath_ = true;
    go2Point(path_.pop());
}

void IRoomba::ignorePath()
{
    path_.clear();

    for(unsigned int i = 0; i < pathLines_.size(); ++i)
    {
        delete pathLines_.at(i);
    }
    pathLines_.clear();
}

//function for comparing vertices
bool IRoomba::verticeCompare(Util::Vertice* first, Util::Vertice* second)
{
    return first->dist > second->dist;
}

IRoomba::~IRoomba()
{

}

}
