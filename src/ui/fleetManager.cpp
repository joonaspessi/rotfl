#include "fleetManager.h"
#include "croi/roombaRoowifi.h"
#include <QDebug>

FleetManager::FleetManager(MainWindow* mainWindow, QObject *parent):
    QObject(parent), mainWindow_(mainWindow), selectedRoomba_(NULL), map_(NULL)
{
}


//map_ cannot be set in constructor because map_ is constructed after
//FleetManager
void FleetManager::setMap(MapQGraphicsView* map)
{
    map_ = map;
}

void FleetManager::sensorUpdateTimerTimeout()
{
    //qDebug() << "sensorUpdateTimerTimeout";
    if(roombas_.empty()) //in case timer is on and there is no roombas
    {
        return;
    }

    for(int i = 0; i < roombas_.size(); ++i)
    {
        roombas_.at(i)->updateState();
    }
    map_->updateLoc(&roombas_);

    mainWindow_->setRoombaStatusData(selectedRoomba_);

    //rest of the function is selection logic upon cursor select.
    //Always makes selectedRoomba_'s starting point and roomba-icon selected
    //if either one is selected by cursor but does nothing if nothing is
    //selected. changes selectedRoomba_ if it isn't selected and another roomba
    //is selected (either start point or roomba).
    //no NULL-pointer check needed if updateLoc already called
    if(selectedRoomba_->getPolygon()->isSelected())
    {
       selectedRoomba_->getStartPoint()->setSelected(true);
    }
    else if(selectedRoomba_->getStartPoint()->isSelected())
    {
        selectedRoomba_->getPolygon()->setSelected(true);
    }
    else //neither is selected by cursor
    {
        for (int i = 0; i < roombas_.size(); ++i)
        {
            //selected roomba found
            if(roombas_.at(i)->getPolygon()->isSelected() ||
               roombas_.at(i)->getStartPoint()->isSelected())
            {
                selectedRoomba_ = roombas_.at(i);
                //both the roomba and it's startPoint are selected for clarity
                selectedRoomba_->getPolygon()->setSelected(true);
                selectedRoomba_->getStartPoint()->setSelected(true);
                //
                break;
            }
        }
    }

    //unselects other roombas that are selected by cursor
    for (int i = 0; i < roombas_.size(); ++i)
    {
        if(roombas_.at(i) != selectedRoomba_)
        {
            roombas_.at(i)->getPolygon()->setSelected(false);
            roombas_.at(i)->getStartPoint()->setSelected(false);
        }
    }
}

void FleetManager::createRoomba(PoiQGraphicsEllipseItem *startPoint)
{
    selectedRoomba_ = new Croi::RoombaRoowifi(startPoint, map_, this);
    //selectedRoomba_ = new Croi::RoombaSerial();
    //TODO: selectedRoomba_ = new Croi::RoombaVirtual();
    roombas_.append(selectedRoomba_);
}

void FleetManager::addPoi(PoiQGraphicsEllipseItem* poi)
{
    pois_.insert(poi);
}

void FleetManager::addWall(WallQGraphicsLineItem* wall)
{
    walls_.insert(wall);
}

void FleetManager::removeRedObjects()
{
    for (std::set<PoiQGraphicsEllipseItem*>::iterator i = pois_.begin();
         i != pois_.end(); ++i)
    {
        if ((*i)->pen().color() == Qt::GlobalColor::red)
        {
            map_->scene()->removeItem(*i);
            delete *i;
            pois_.erase(i);
        }
    }
    for (std::set<WallQGraphicsLineItem*>::iterator i = walls_.begin();
         i != walls_.end(); ++i)
    {
        if ((*i)->pen().color() == Qt::GlobalColor::red)
        {
            map_->scene()->removeItem(*i);
            delete *i;
            walls_.erase(i);
        }
    }

    //THE STARTING POINT CAN'T BE REMOVED NOW (TEMPORARY CHANGE)
    //all tracking and tracing is taken away if startPoint_ is removed
    //    if (startPoint_ != NULL && startPoint_->pen().color() == Qt::GlobalColor::red)
    //    {
    //        delete startPoint_;
    //        startPoint_ = NULL;
    //        removeTraces();
    //        scene()->removeItem(curPoint_);
    //        delete curPoint_;
    //        curPoint_ = NULL;
    //        scene()->removeItem(curSpeed_);
    //        delete curSpeed_;
    //        curSpeed_ = NULL;
    //        initX_= 0.0;
    //        initY_= 0.0;

    //    }
}

void FleetManager::removePoi(PoiQGraphicsEllipseItem* poi)
{
    map_->scene()->removeItem(poi);
    pois_.erase(poi);
    delete poi;
}

//shows or hides all traces
void FleetManager::ifShowTraces()
{
    for (int i = 0; i < roombas_.size(); ++i)
    {
        roombas_.at(i)->ifShowTraces();
    }
}

//removes selectedRoomba's traces
void FleetManager::removeTraces()
{
    selectedRoomba_->removeTraces();
}

void FleetManager::go2Poi()
{
//NEEDS CHANGES AND FURTHER IMPLEMENTATION
//    QPointF poiCoordinate = FUNCTION GETNEXTPOI IS NO MORE
//    QPointF roombaCoordinate = roombas_.at(0)->getRoombasLocation();
//    qDebug() << "POI coordinate x: " << poiCoordinate.x()
//             << " , y: " << poiCoordinate.y();
//    qDebug() << "Roomba coordinate x: " << roombaCoordinate.x()
//             << " , y: " << roombaCoordinate.y();
//    float deltaX = abs(roombaCoordinate.x() - poiCoordinate.x());
//    float deltaY = abs(roombaCoordinate.y() - poiCoordinate.y());
//    float angleRadian = atan2(deltaY, deltaX);
//    //float anglePi = angleRadian*180 / PI;

//    //stop
//    selectedRoomba_->Drive(0,32767);
//    radius_ = 32767;
//    //moving_ = false;

//    double initialAngle = roombas_.at(0)->getAngle();



//    //calculate
}

void FleetManager::checkPoiCollision()
{
    for (std::set<PoiQGraphicsEllipseItem*>::iterator i = pois_.begin();
         i != pois_.end(); ++i)
    {
        QList<QGraphicsItem*> collidingItems = (*i)->collidingItems();
        if (!collidingItems.empty())
        {
            removePoi(*i);
        }
    }
}

void FleetManager::connect(std::string stdip)
{
    //    Disabled until Roowifi AutoCapture is used instead
    //    updateSensorData_->start(500);
    selectedRoomba_->rmb_connect(stdip);
}

void FleetManager::disconnect()
{
    //    Disabled until Roowifi AutoCapture is used instead
    //    updateSensorData_->stop();
    selectedRoomba_->disconnect();
}

void FleetManager::clean()
{
    selectedRoomba_->clean();
}

void FleetManager::allMotorsOn()
{
    selectedRoomba_->allMotorsOn();
}

void FleetManager::allMotorsOff()
{
    selectedRoomba_->allMotorsOff();
}

void FleetManager::safeMode()
{
    selectedRoomba_->safeMode();
}

void FleetManager::fullMode()
{
    selectedRoomba_->fullMode();
}

void FleetManager::resetAngle()
{
    selectedRoomba_->resetAngle();
}

void FleetManager::playSong(unsigned int songNum)
{
    selectedRoomba_->playSong(songNum);
}

void FleetManager::setVelocity(int velocity)
{
    if (selectedRoomba_->getVelocity() != 0)
    {
        selectedRoomba_->drive(velocity);
    }
}

void FleetManager::drive( int velocity, int radius )
{
    selectedRoomba_->drive(velocity, radius);
}

void FleetManager::drive( int velocity)
{
    selectedRoomba_->drive(velocity);
}

FleetManager::~FleetManager()
{}
