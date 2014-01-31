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
#include <QMessageBox>
#include "roombaVirtual.h"

namespace Croi {

IRoomba::IRoomba(PoiQGraphicsEllipseItem *startPoint, MapQGraphicsView *map,
                 FleetManager* parent):
    QObject(parent),
    startPoint_(startPoint), map_(map), icon_(NULL), Xloc_(startPoint->x()),
    Yloc_(startPoint->y()), angle_(0.0), radius_(Util::RADSTRAIGHT),
    velocity_(0), traceShown_(true), isReady_(false), driveTime_(0),
    followingPath_(false), prevPReached_(false), destPoi_(NULL), totalDistance_(0),
    cleaning_(false), destAtc_(NULL)
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

void IRoomba::clean()
{
    followingPath_ = true;
    cleaning_ = true;
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

PoiQGraphicsEllipseItem* IRoomba::getDestPoi()
{
    return destPoi_;
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
    bool leftBump = getLeftBumb();
    bool rightBump = getRightBumb();

    QPointF p = calcLoc();
    double x = p.x();
    double y = p.y();

    if(round(Xloc_) != round(x) || round(Yloc_) != round(y))
    {
        if(cleaning_ && !followingPath_)
        {
            drawTrace(x, y, Qt::GlobalColor::green);
        }
        else
        {
            drawTrace(x, y, Qt::GlobalColor::gray);
        }
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
        FleetManager *fleetManager = qobject_cast<FleetManager*>(parent());

        if(destPoi_ != NULL || cleaning_)
        {
            fleetManager->stopFleet(true);
        }
        else
        {
            double tempAngle = angle_-40.0*Util::PI/180.0;
            QPointF l (Xloc_+cos(tempAngle)*17, Yloc_+sin(tempAngle)*17);
            tempAngle = angle_+40.0*Util::PI/180.0;
            QPointF r (Xloc_+cos(tempAngle)*17, Yloc_+sin(tempAngle)*17);
            WallQGraphicsLineItem* bumbed = new WallQGraphicsLineItem
                                                (fleetManager, l.x(), l.y(), r.x(), r.y());
            map_->scene()->addItem(bumbed);
            fleetManager->addWall(bumbed);
        }
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

void IRoomba::drawTrace(double x, double y, Qt::GlobalColor color)
{
    //new piece of trace is created (polygon_ set as parent)
    if (color == Qt::GlobalColor::green)
    {
        QGraphicsLineItem* ctraceL = new QGraphicsLineItem
                                        (Xloc_, Yloc_, x, y);
        QPen clinePen(color);
        clinePen.setWidth(Util::PIXELCLEANWIDTH);
        ctraceL->setPen(clinePen);
        ctraceL->setOpacity(0.25);
        ctraces_.append(ctraceL);
        map_->scene()->addItem(ctraceL);
    }
    else
    {
        QGraphicsLineItem* traceL = new QGraphicsLineItem
                                        (Xloc_, Yloc_, x, y);
        QPen linePen(color);

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
}



QPointF IRoomba::calcLoc()
{
    //subclass handles the retrieval of sensor information
    double distance = getDistance()/Util::COORDCORRECTION;
    totalDistance_ += distance;

    double angle = getAngle();

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

    return QPointF(x,y);
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

    float tabs = fabs(turningAngle*(180.0/Util::PI));
    float distance = sqrt(pow(deltaX,2.0)+pow(deltaY,2.0))*Util::COORDCORRECTION;
    int turnTime = round(tabs*TURNTIMEINUSFORDEG/1000.0);
    //driveTime_ is calculated. It needs to be corrected with a real roomba
    //by a factor of around 1.3
    if(qobject_cast<RoombaVirtual*>(this))
    {
        driveTime_= round(distance*100.0*100.0/FWSPEED);
    }
    else
    {
        driveTime_= (distance*100.0*100.0/FWSPEED)*0.85;
    }

    qDebug() << "turningAngle: " << tabs << "ttime: " << turnTime;
    qDebug() << "distance: " << distance << "dtime: " << driveTime_;
    //QCoreApplication::processEvents(QEventLoop::AllEvents, 1);


//    qDebug() << "Turning angle in degrees: " << turningAngle*(180/PI);

    if (turningAngle > 0) //Turn clockwise
    {
        this->drive(100,Util::RADTURNCW);
        turnDirection_= Util::RADTURNCW;

    }
    else
    {
        this->drive(100,Util::RADTURNCCW);
        turnDirection_= Util::RADTURNCCW;

    }
    QTimer::singleShot(turnTime,Qt::PreciseTimer, this, SLOT(turnTimerTimeout()));


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

        if(destPoi_ != NULL)
        {
            isReady_ = true;
            PoiQGraphicsEllipseItem* tempPoi = destPoi_;
            destPoi_ = NULL;
            qobject_cast<FleetManager*>(parent())->poiCollected(this, tempPoi);
        }
        else if(cleaning_)
        {
            calc4square(m_sy);
            squareStart();  //rectangle size, height and width is decided here.
        }
    }
    else
    {
        prevPReached_ = false;
        go2Point(path_.pop());
    }
}

void IRoomba::turnTimerTimeout()
{
    drive(0,32767);
    drive(FWSPEED, Util::RADSTRAIGHT);
    QTimer::singleShot(driveTime_, Qt::PreciseTimer, this, SLOT(driveTimerTimeout()));

}

void IRoomba::driveTimerTimeout()
{
    drive(0,32767);
    prevPReached_ = true;
}

void IRoomba::squareStart() //after reaching to the starting point, turn angle horizontally.
{
    m_count=0;

//    float time=(abs(angle_*(180/PI)))*18055/1000;

    float time=(fabs(angle_*(180.0/PI)))*18055/1000;
    qDebug() << "5. squareStart time=turnTime: " << time;
    /*
    if (turnDirection_== Util::RADTURNCW){
        turnDirection_= Util::RADTURNCCW;
        qDebug()<<"RADTURNCW 1, angle_ is "<<angle_;

    } else {
        turnDirection_= Util::RADTURNCW;
        qDebug()<<"RADTURNCCW 2, angle_ is"<<angle_;
        time=(fabs((2*PI-angle_)*(180.0/PI)))*18055/1000;
    }
    */
    if (angle_<= PI) {
        turnDirection_= Util::RADTURNCCW;
        qDebug()<<"RADTURNCW 1, angle_ is "<<angle_;

    } else {
        turnDirection_= Util::RADTURNCW;
        qDebug()<<"RADTURNCCW 2, angle_ is"<<angle_;
        time=(fabs((2*PI-angle_)*(180.0/PI)))*18055/1000;

    }

    drive(100, turnDirection_);
    QTimer::singleShot(time,Qt::PreciseTimer, this,SLOT(rotateEnded()));
    allMotorsOn();
    qDebug() << "6. squarestart singleShot time: " << time;

}

void IRoomba::rotateEnded()     //turning ended, start to drive straight, distance the width-tracewidth/2
{
    qDebug()<<"angle_ before cleaning is "<<angle_;
    //checking angle_ before starting cleaning. angle_ has to be approx. 0.
/*
    if (angle_ > 0.01 && angle_ <2 && (countDown == nOfRound)){
        angleCorrection();
    } else if (angle_ > 5 && ((2*PI-angle_)>0.01) && (countDown == nOfRound)) {
        angleCorrection();
    } else {
*/
        drive(0, turnDirection_);
        drive(100, Util::RADSTRAIGHT);
        QTimer::singleShot((m_sx-Util::REALCLEANWIDTH-1.5)*100, Qt::PreciseTimer, this, SLOT(squareTurn()));

}

/*
void IRoomba::angleCorrection()
{
    float time=(fabs(angle_*(180.0/PI)))*18055/1000;

    if (angle_<= PI) {
        turnDirection_= Util::RADTURNCCW;
        qDebug()<<"RADTURNCW 1, angle_ is "<<angle_;

    } else {
        turnDirection_= Util::RADTURNCW;
        qDebug()<<"RADTURNCCW 2, angle_ is"<<angle_;
        time=(fabs((2*PI-angle_)*(180.0/PI)))*18055/1000;

    }

    drive(100, turnDirection_);
    QTimer::singleShot(time, Qt::PreciseTimer,this,SLOT(rotateEnded()));

}
*/

void IRoomba::squareTurn()    //1st line end, stop and turn 90 degree
{
    drive(0, Util::RADSTRAIGHT);
    drive(100, Util::RADTURNCW);

    int turnTime = abs(90 /**(180/PI)*/) * 18055 / 1000;

    QTimer::singleShot(turnTime,Qt::PreciseTimer, this, SLOT(squareMoveOneLine()));

}

void IRoomba::squareMoveOneLine() //move 1st line to 2nd line with distance height/5, here is 10
{
    drive(0, Util::RADTURNCW);
    drive(100, Util::RADSTRAIGHT);

    QTimer::singleShot(ld*100,Qt::PreciseTimer, this, SLOT(squareTurn2()));

}

void IRoomba::squareTurn2() //stop and turn 90 degree in the beginning of 2nd line
{
    drive(0, Util::RADSTRAIGHT);
    drive(100, Util::RADTURNCW);

    int turnTime = abs(90/**(180/PI)*/) * 18055 / 1000;

    QTimer::singleShot(turnTime,Qt::PreciseTimer, this, SLOT(squareMoveLong()));

}

void IRoomba::squareMoveLong()
{
    drive(100, Util::RADSTRAIGHT);
    QTimer::singleShot((m_sx-Util::REALCLEANWIDTH-1.5)*100, Qt::PreciseTimer,this, SLOT(squareTurn3()));
}



void IRoomba::squareTurn3()
{
    drive(0, Util::RADSTRAIGHT);
    drive(100, Util::RADTURNCCW);

    int turnTime = abs(90/**(180/PI)*/) * 18055 / 1000;

    if ((--countDown)== 0) {
        cleaning_=false;
        qDebug() << "area cleaned, check next angle to move to new POI ";
        drive(0, Util::RADSTRAIGHT);
        allMotorsOff();

        int turnTime = abs(180/**(180/PI)*/) * 18055 / 1000;

        QTimer::singleShot(turnTime, Qt::PreciseTimer, this, SLOT(squareStart2()));


    }
    else {
        QTimer::singleShot(turnTime, Qt::PreciseTimer, this, SLOT(squareMoveOneLine2()));
    }
    qDebug()<<"countDown is "<<countDown;
}

void IRoomba::squareMoveOneLine2() //move outside of the square,
{
    drive(0, Util::RADTURNCW);

    drive(100, Util::RADSTRAIGHT);

    QTimer::singleShot(ld*100, Qt::PreciseTimer, this, SLOT(squareTurn4()));

}

void IRoomba::squareTurn4()   //at outside of the square, stop, turn 90 degree anti-clock
{
    drive(0, Util::RADSTRAIGHT);
    drive(100, Util::RADTURNCCW);

    int turnTime = abs(90/**(180/PI)*/) * 18055 / 1000;

    QTimer::singleShot(turnTime,Qt::PreciseTimer, this, SLOT(squareStart2()));


}

void IRoomba::squareStart2()
{
  m_count++;
  if (m_count < nOfRound) {  //how many round to try,1 round is 2 lines
      drive(100, Util::RADSTRAIGHT);
      QTimer::singleShot((m_sx-Util::REALCLEANWIDTH-1.5)*100,Qt::PreciseTimer,this, SLOT(squareTurn()));
  }
  else
  {
      //drive(0, Util::RADTURNCCW);

      //allMotorsOff();
      //cleaning_ = false;
      //qobject_cast<FleetManager*>(parent())->removeAtc(destAtc_);
      isReady_ = true;
      qobject_cast<FleetManager*>(parent())->atcCleaned(this);
      // emit areaCleaned();
      //qDebug() << "area cleaned, check next angle to move to new POI ";
  }

}

void IRoomba::calc4square(int h)
{
       //always even num of trace width, 1 round is 2 Util::PIXELCLEANWIDTH,
       //ld=line distances

    if (h>0 && h<=1000) {
        nOfRound = h/(2*Util::REALCLEANWIDTH)+1;  //or this, nOfRound = (h>>3)/5+1
        if (h%(2*Util::REALCLEANWIDTH)==0)
            nOfRound = h/(2*Util::REALCLEANWIDTH);
    } else {
       qDebug() <<"too big";
    }

//    ld = (abs(h-Util::REALCLEANWIDTH/2))/(2*nOfRound);
//    ld = h/(2*nOfRound);
      if (h> 2*Util::REALCLEANWIDTH)
            ld = (h-Util::REALCLEANWIDTH-1.5)/(2.0*nOfRound-1.0);

      else if ((h>Util::REALCLEANWIDTH) && (h<=2*Util::REALCLEANWIDTH))
            ld = h-Util::REALCLEANWIDTH-1.5;
      else
            ld = 0;

      countDown= nOfRound;

    qDebug()<<"m_sx is "<<m_sx;
    qDebug()<<"m_sy is "<<m_sy;
    qDebug()<<"h is "<<h;
    qDebug()<<"nOfRound is "<<nOfRound;
    qDebug()<<"ld is "<<ld;
    qDebug()<<"countDown is"<<countDown;

}

void IRoomba::setSquare(int w, int h) {
    m_sx = w*Util::COORDCORRECTION;
    m_sy = h*Util::COORDCORRECTION;
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
    for(int i = 0; i < vertices.size(); ++i)
    {
        for(int j = 0; j < vertices.at(0).size(); ++j)
        {
//            qDebug() << "verticeX: " << vertices.at(i).at(j)->topLeftX << " "
//                     << "verticeY: " << vertices.at(i).at(j)->topLeftY;
            if(unsigned(floor(Xloc_)) >= vertices.at(i).at(j)->topLeftX &&
               unsigned(floor(Xloc_)) <= vertices.at(i).at(j)->topLeftX+Util::VERTICEWIDTH-1 &&
               unsigned(floor(Yloc_)) >= vertices.at(i).at(j)->topLeftY &&
               unsigned(floor(Yloc_)) <= vertices.at(i).at(j)->topLeftY+Util::VERTICEWIDTH-1)
            {
                startV = vertices.at(i).at(j);
            }
            if(unsigned(floor(point.x())) >= vertices.at(i).at(j)->topLeftX &&
               unsigned(floor(point.x())) <= vertices.at(i).at(j)->topLeftX+Util::VERTICEWIDTH-1 &&
               unsigned(floor(point.y())) >= vertices.at(i).at(j)->topLeftY &&
               unsigned(floor(point.y())) <= vertices.at(i).at(j)->topLeftY+Util::VERTICEWIDTH-1)
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
    if(startV == NULL)
    {
        qDebug() << "Crash Xloc_: " << Xloc_
                 << "Crash Yloc_: " << Yloc_;
    }
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
    isReady_ = false;
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
    isReady_ = true;
    drive(0, Util::RADSTRAIGHT);

    path_.clear();

    for(unsigned int i = 0; i < pathLines_.size(); ++i)
    {
        map_->scene()->removeItem(pathLines_.at(i));
        delete pathLines_.at(i);
    }
    pathLines_.clear();
    destPoi_ = NULL;
    destAtc_ = NULL;
    cleaning_ = false;

    m_sx = 0;
    m_sy = 0;
    m_count = 0;
    nOfRound = 0;
    ld = 0.0;
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

void IRoomba::setDestAtc(AtcQGraphicsRectItem* atc)
{
    destAtc_ = atc;
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
