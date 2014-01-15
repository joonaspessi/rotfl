#include "roombaVirtual.h"
#include "croiUtil.h"
#include "mapQGraphicsView.h"
#include <QDebug>

namespace Croi {

RoombaVirtual::RoombaVirtual(PoiQGraphicsEllipseItem *startPoint,
                             MapQGraphicsView* map, FleetManager *parent):
    IRoomba(startPoint, map, parent), deltaDistance_(0.0), deltaAngle_(0.0),
    lastDriveCmdTime_(0)
{
    statusTimer_ = new QTimer(this);
    IRoomba::updateState(); //call needed to have no null pointer in icon_
    rmb_connect("");  //ip is irrelevant
    IRoomba::safeMode();
}

RoombaVirtual::~RoombaVirtual()
{
}

int RoombaVirtual::rmb_connect(std::string ip)
{
    connect(statusTimer_, SIGNAL(timeout()), this, SLOT(statusTimerTimeout()));
    statusTimer_->setSingleShot(false);
    statusTimer_->start(100);
}

int RoombaVirtual::disconnect()
{
    statusTimer_->stop();
    IRoomba::disconnect();
}

void RoombaVirtual::allMotorsOn()
{
}

void RoombaVirtual::allMotorsOff()
{
}

void RoombaVirtual::clean()
{
    IRoomba::clean();
}

char RoombaVirtual::getTemperature()
{
    return 0;
}

unsigned short RoombaVirtual::getChargeLevel()
{
    return 0;
}

double RoombaVirtual::getDistance()
{
    double tmp = deltaDistance_;
    deltaDistance_ = 0.0;
    return tmp;
}

double RoombaVirtual::getAngle()
{
    double tmp = deltaAngle_;
    deltaAngle_ = 0.0;
    return tmp;
}

bool RoombaVirtual::getLeftBumb()
{
    return false;
}

bool RoombaVirtual::getRightBumb()
{
    return false;
}

void RoombaVirtual::drive(int velocity, int radius)
{
    if(IRoomba::getVelocity() != 0)
    {
        //previous movement needs to be recorded
        switch (IRoomba::getRadius()) {
        case Util::RADSTRAIGHT:
            deltaDistance_ -= (statusTimer_->interval()-statusTimer_->remainingTime()-lastDriveCmdTime_)*IRoomba::getVelocity()/1000.0/DISTANCECORRECTION;
            break;
        case Util::RADTURNCW:
            deltaAngle_ -= (statusTimer_->interval()-statusTimer_->remainingTime()-lastDriveCmdTime_)*IRoomba::getVelocity()/100.0/(TURNTIMEINUSFORDEG/1000.0)/ANGLECORRECTION;
            break;
        case Util::RADTURNCCW:
            deltaAngle_ += (statusTimer_->interval()-statusTimer_->remainingTime()-lastDriveCmdTime_)*IRoomba::getVelocity()/100.0/(TURNTIMEINUSFORDEG/1000.0)/ANGLECORRECTION;
            break;
        default:
              //Below code for radial movement doesn't work
//            double deltaDeltaDistance = (statusTimer_->interval()-statusTimer_->remainingTime()-lastDriveCmdTime_)*IRoomba::getVelocity()/DISTANCECORRECTION;
//            deltaDistance_ -= deltaDeltaDistance;
//            deltaAngle_ -= deltaDeltaDistance/(IRoomba::getRadius()*2*Util::PI);
            break;
        }
    }
    lastDriveCmdTime_ = statusTimer_->interval()-statusTimer_->remainingTime();
    IRoomba::drive(velocity, radius);
}

void RoombaVirtual::drive(int velocity)
{
    if(IRoomba::getVelocity() != 0)
    {
        //previous movement needs to be recorded
        switch (IRoomba::getRadius()) {
        case Util::RADSTRAIGHT:
            deltaDistance_ -= (statusTimer_->interval()-statusTimer_->remainingTime()-lastDriveCmdTime_)*IRoomba::getVelocity()/1000.0/DISTANCECORRECTION;
            break;
        case Util::RADTURNCW:
            deltaAngle_ -= (statusTimer_->interval()-statusTimer_->remainingTime()-lastDriveCmdTime_)*IRoomba::getVelocity()/100.0/(TURNTIMEINUSFORDEG/1000.0)/ANGLECORRECTION;
            break;
        case Util::RADTURNCCW:
            deltaAngle_ += (statusTimer_->interval()-statusTimer_->remainingTime()-lastDriveCmdTime_)*IRoomba::getVelocity()/100.0/(TURNTIMEINUSFORDEG/1000.0)/ANGLECORRECTION;
            break;
        default:
              //Below code for radial movement doesn't work
//            double deltaDeltaDistance = (statusTimer_->interval()-statusTimer_->remainingTime()-lastDriveCmdTime_)*IRoomba::getVelocity()/DISTANCECORRECTION;
//            deltaDistance_ -= deltaDeltaDistance;
//            deltaAngle_ -= deltaDeltaDistance/(IRoomba::getRadius()*2*Util::PI);
            break;
        }
    }
    lastDriveCmdTime_ = statusTimer_->interval()-statusTimer_->remainingTime();
    IRoomba::drive(velocity);
}

void RoombaVirtual::playSong(int songNumber)
{
}

void RoombaVirtual::statusTimerTimeout()
{
    if(IRoomba::getVelocity() != 0)
    {
        //previous movement needs to be recorded
        switch (IRoomba::getRadius()) {
        case Util::RADSTRAIGHT:
            deltaDistance_ -= (statusTimer_->interval()-lastDriveCmdTime_)*IRoomba::getVelocity()/1000.0/DISTANCECORRECTION;
            break;
        case Util::RADTURNCW:
            deltaAngle_ -= (statusTimer_->interval()-lastDriveCmdTime_)*IRoomba::getVelocity()/100.0/(TURNTIMEINUSFORDEG/1000.0)/ANGLECORRECTION;
            break;
        case Util::RADTURNCCW:
            deltaAngle_ += (statusTimer_->interval()-lastDriveCmdTime_)*IRoomba::getVelocity()/100.0/(TURNTIMEINUSFORDEG/1000.0)/ANGLECORRECTION;
            break;
        default:
              //Below code for radial movement doesn't work
//            double deltaDeltaDistance = (statusTimer_->interval()-lastDriveCmdTime_)*IRoomba::getVelocity()/DISTANCECORRECTION;
//            deltaDistance_ -= deltaDeltaDistance;
//            deltaAngle_ -= deltaDeltaDistance/(IRoomba::getRadius()*2*Util::PI);
            break;
        }
    }
    lastDriveCmdTime_ = 0;
    IRoomba::sensorUpdateTimerTimeout();
    //these currently nulled in get-functions
    //deltaDistance_ = 0.0;
    //deltaAngle_ = 0.0;
}

QGraphicsPixmapItem* RoombaVirtual::setIcon()
{
    // TODO: Improve icon graphics before deploying
    QPixmap pixmap(":/icons/DEBUG_roomba_virtual_small");
    QGraphicsPixmapItem* icon = IRoomba::getMap()->scene()->addPixmap(pixmap);
    icon->setOffset(icon->pixmap().width()/-2.0, icon->pixmap().width()/-2.0);
    icon->setPos(IRoomba::getLoc().x(), IRoomba::getLoc().y());
    icon->setFlag(QGraphicsItem::ItemIsSelectable, true);
    icon->setFlag(QGraphicsItem::ItemIsMovable,false);
    icon->setScale(1.0/Util::COORDCORRECTION);
    return icon;
}

void RoombaVirtual::goDock()
{
//TODO: go to starting point
}

float RoombaVirtual::getBatteryLevel()
{
    return 0.0;
}

} //namespace Croi
