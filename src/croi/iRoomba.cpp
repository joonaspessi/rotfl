#include "iRoomba.h"
#include "mapQGraphicsView.h"
#include "fleetManager.h"
#include "QDebug"
#include <QObject>
#include <queue>
#include <vector>
#include <cmath>
#include <limits>
#include "croiUtil.h"

namespace Croi {

IRoomba::IRoomba(PoiQGraphicsEllipseItem *startPoint, MapQGraphicsView *map,
                 FleetManager* parent):
    QObject(parent),
    startPoint_(startPoint), map_(map), icon_(NULL), Xloc_(startPoint->x()),
    Yloc_(startPoint->y()), angle_(0.0), radius_(Util::RADSTRAIGHT),
    velocity_(0), traceShown_(true), isReady_(false), driveTime_(0),
    followingPath_(false), prevPReached_(false), destPoi_(NULL), totalDistance_(0)
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

double IRoomba::getTotalDistance()
{
    return totalDistance_/100;
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

void IRoomba::correctLocation(Util::Direction direction)
{
    switch (direction) {
    case Util::Direction::W:
        Xloc_ -= Util::ONTHEFLYCORRECTION;
        break;
    case Util::Direction::E:
        Xloc_ += Util::ONTHEFLYCORRECTION;
        break;
    case Util::Direction::N:
        Yloc_ -= Util::ONTHEFLYCORRECTION;
        break;
    case Util::Direction::S:
        Yloc_ += Util::ONTHEFLYCORRECTION;
        break;
    default:
        break;
    }
}

void IRoomba::correctAngle(bool clockWise)
{
    if(clockWise)
    {
        angle_ += Util::ONTHEFLYCORRECTION*PI/180.0;
    }
    else
    {
        angle_ -= Util::ONTHEFLYCORRECTION*PI/180.0;
    }
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

MapQGraphicsView* IRoomba::getMap()
{
    return map_;
}

void IRoomba::updateState()
{
    //subclass handles the retrieval of sensor information
    double distance = getDistance()/Util::COORDCORRECTION;
    totalDistance_ += distance;

    double angle = getAngle();
    bool leftBump = getLeftBumb();
    bool rightBump = getRightBumb();

    //angle for distance calculation
    double angleForDist = angle_-angle*Util::PI*ANGLECORRECTION/180.0;
    //distance changed to cm
    double dist = -distance/10.0*DISTANCECORRECTION;
    //special radiuses mean no adaptation needed
    if (radius_ != 32768 && radius_ != 32767 && radius_ != -1 && radius_ != 1)
    {
        //corrected distance (and change to cm)
        dist = -2.0*(static_cast<double>(radius_))*
                sin(distance/radius_/2)/10.0*DISTANCECORRECTION;
        //corrected angle in radians for distance calculation
        //angleForDist = angle*PI/180.0*ANGLECORRECTION/2.0;
        //other version
        angleForDist = angle_-distance/radius_/2.0;
    }
    //real angle (always used for roomba's angle)
    angle_ -= angle*Util::PI*ANGLECORRECTION/180.0;
    //qDebug() << "Roomba's angle in degrees: " << angle_*(180/PI);
    angle_ = fmod(angle_, 2.0*Util::PI);
    if (angle_ < 0)
    {
        angle_ = 2*Util::PI+angle_;
    }
    //qDebug() << "Roomba's angle in degrees after correction: " << angle_*(180/PI);

    //coordinates are updated
    double x = Xloc_+cos(angleForDist)*dist;
    double y = Yloc_+sin(angleForDist)*dist;

    if(round(Xloc_) != round(x) || round(Yloc_) != round(y))
    {
        //new piece of trace is created
        QGraphicsLineItem* traceL = new QGraphicsLineItem
                (Xloc_, Yloc_, x, y);
        QPen linePen(Qt::GlobalColor::gray);
        linePen.setWidth(Util::TRACEWIDTH);
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
        icon_ = setIcon();
    }
    else
    {
        icon_->setPos(Xloc_, Yloc_);
        icon_->resetTransform();
        // Roomba icon is in angle of 270 by default in the pixmap, setRotation accepts also negative angles
        icon_->setRotation((angle_*(180/Util::PI))-270);
    }
    icon_->setZValue(2);

    //add new wall if bumb has happened
    if (leftBump || rightBump)
    {
        qDebug() << "Roomba has collided with unknown object!";
        double tempAngle = angle_-40.0*Util::PI/180.0;
        QPointF l (Xloc_+cos(tempAngle)*17, Yloc_+sin(tempAngle)*17);
        tempAngle = angle_+40.0*Util::PI/180.0;
        QPointF r (Xloc_+cos(tempAngle)*17, Yloc_+sin(tempAngle)*17);
        FleetManager *fleetManager = qobject_cast<FleetManager*>(parent());
        WallQGraphicsLineItem* bumbed = new WallQGraphicsLineItem
                                            (fleetManager, l.x(), l.y(), r.x(), r.y());
        map_->scene()->addItem(bumbed);
        fleetManager->addWall(bumbed);
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

    if (roombasAngle > Util::PI)
    {
        roombasAngle -= 2*Util::PI;
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
            if (calculatedAngle -roombasAngle < (-1*Util::PI) )
            {
                turningAngle = calculatedAngle - roombasAngle + 2*Util::PI;
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
            if ( calculatedAngle - roombasAngle > Util::PI )
            {
                turningAngle = calculatedAngle - roombasAngle - 2*Util::PI;
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

    float tabs = abs(turningAngle*(180.0/Util::PI));
    float distance = sqrt(pow(deltaX,2.0)+pow(deltaY,2.0))*Util::COORDCORRECTION;
    int turnTime = round(tabs*TURNTIMEINUSFORDEG/1000.0);
    driveTime_= round(distance*100.0);

    qDebug() << "turningAngle: " << tabs << "ttime: " << turnTime;
    qDebug() << "distance: " << distance << "dtime: " << driveTime_;
    //QCoreApplication::processEvents(QEventLoop::AllEvents, 1);


//    qDebug() << "Turning angle in degrees: " << turningAngle*(180/PI);

    if (turningAngle > 0) //Turn clockwise
    {
        this->drive(100,Util::RADTURNCW);

    }
    else
    {
        this->drive(100,Util::RADTURNCCW);
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

        qobject_cast<FleetManager*>(parent())->removePoi(destPoi_);
        destPoi_ = NULL;
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
    this->drive(100, Util::RADSTRAIGHT);
    QTimer::singleShot(driveTime_, this, SLOT(driveTimerTimeout()));

}

void IRoomba::driveTimerTimeout()
{
    this->drive(0,32767);
    prevPReached_ = true;
}

double IRoomba::calcPath(QVector<QVector<Util::Vertice *> > &vertices, QPointF point)
{
    //first we try to find a straight path without turns caused by dividing map to
    //vertices. For that we use a testLine and check if it collides with a wall. If no
    //colliding wall is found, point is pushed to path and its distance returned.
    //Otherwise, we proceed with using Dijkstra's algorithm.
    QGraphicsLineItem *testLine = new QGraphicsLineItem
                                      (Xloc_, Yloc_, point.x(), point.y());
    QPen testLinePen;
    testLinePen.setWidth(Util::ROOMBAWIDTH+6); //6 is to have some leeway due to inaccuracies
    testLine->setPen(testLinePen);
    map_->scene()->addItem(testLine);
    QList<QGraphicsItem*> colList = testLine->collidingItems();
    map_->scene()->removeItem(testLine);
    delete testLine;
    bool wallFound = false;

    for(QList<QGraphicsItem*>::iterator i = colList.begin(); i != colList.end(); ++i)
    {
        if((*i)->type() == Util::WALLTYPE)
        {
            wallFound = true;
        }
    }

    if(!wallFound)
    {
        double deltaX = point.x()-Xloc_;
        double deltaY = point.y()-Yloc_;
        double distance = sqrt(pow(deltaX,2.0)+pow(deltaY,2.0));

        //due to inaccuracies, the straight path is divided into several points. This
        //allows roomba to stay on course accurately enough.
        path_.push(point);
        pathLines_.append(new QGraphicsLineItem(Xloc_, Yloc_, point.x(), point.y()));
        if(distance != 0)
        {
            unsigned int pointCount = ceil(distance/70.0);
            for(unsigned int i = pointCount-1; i > 0; --i)
            {
                path_.push(QPointF(Xloc_+i*deltaX/pointCount, Yloc_+i*deltaY/pointCount));
            }
        }
        return distance;
    }

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

double IRoomba::calcPath(QVector<QVector<Util::Vertice *> > &vertices, PoiQGraphicsEllipseItem *poi)
{
    destPoi_ = poi;
    return calcPath(vertices, poi->pos());
}


void IRoomba::compNeigh(Util::Vertice *curV, Util::Direction direction,
                        std::priority_queue<Util::Vertice *,
                                            std::vector<Util::Vertice *>,
                                            bool (*)(Util::Vertice *, Util::Vertice *)> &priQ)
{
    double dist = Util::VERTICEWIDTH;  //distance between curV and neighV
    Util::Vertice* neighV = NULL;

    switch(direction) //some directions need updated distances
    {
    case Util::N:
        neighV = curV->n;
        break;
    case Util::NE:
        dist = sqrt(2.0*pow(Util::VERTICEWIDTH, 2));
        neighV = curV->ne;
        break;
    case Util::E:
        neighV = curV->e;
        break;
    case Util::SE:
        dist = sqrt(2.0*pow(Util::VERTICEWIDTH, 2));
        neighV = curV->se;
        break;
    case Util::S:
        neighV = curV->s;
        break;
    case Util::SW:
        dist = sqrt(2.0*pow(Util::VERTICEWIDTH, 2));
        neighV = curV->sw;
        break;
    case Util::W:
        neighV = curV->w;
        break;
    case Util::NW:
        dist = sqrt(2.0*pow(Util::VERTICEWIDTH, 2));
        neighV = curV->nw;
        break;
    }

    if(neighV == NULL || neighV->blocked) //no link to neighbour, nothing is done
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
    followingPath_ = true;
    for(unsigned int i = 0; i < pathLines_.size(); ++i)
    {
        map_->scene()->addItem(pathLines_.at(i));
    }
    go2Point(path_.pop());
}

void IRoomba::ignorePath()
{
    path_.clear();
    destPoi_ = NULL;

    for(unsigned int i = 0; i < pathLines_.size(); ++i)
    {
        delete pathLines_.at(i);
    }
    pathLines_.clear();
}

void IRoomba::stop()
{
    followingPath_ = false;
    prevPReached_ = false;
    drive(0, Util::RADSTRAIGHT);

    path_.clear();

    for(unsigned int i = 0; i < pathLines_.size(); ++i)
    {
        map_->scene()->removeItem(pathLines_.at(i));
        delete pathLines_.at(i);
    }
    pathLines_.clear();
    destPoi_ = NULL;
}

QGraphicsPixmapItem* IRoomba::setIcon()
{
    // TODO: Improve icon graphics before deploying
    QPixmap pixmap(":/icons/DEBUG_roomba_small");
    QGraphicsPixmapItem* icon = map_->scene()->addPixmap(pixmap);
    icon->setOffset(icon->pixmap().width()/-2.0, icon->pixmap().width()/-2.0);
    icon->setPos(Xloc_, Yloc_);
    icon->setFlag(QGraphicsItem::ItemIsSelectable, true);
    icon->setFlag(QGraphicsItem::ItemIsMovable,false);
    icon->setScale(1.0/Util::COORDCORRECTION);
    return icon;
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
