#include "mainwindow.h"
#include "fleetManager.h"
#include "croi/roombaRoowifi.h"
#include <QDebug>
#include <QObject>
#include <QMessageBox>

FleetManager::FleetManager(MainWindow* mainWindow, QObject *parent):
    QObject(parent), mainWindow_(mainWindow), map_(NULL)
{
    updateTimer_ = new QTimer(this);
    QObject::connect(updateTimer_, SIGNAL(timeout()), this, SLOT(updateTimerTimeout()));
    updateTimer_->setSingleShot(false);
    updateTimer_->start(100);
}


//map_ cannot be set in constructor because map_ is constructed after
//FleetManager
void FleetManager::setMap(MapQGraphicsView* map)
{
    map_ = map;
}

void FleetManager::updateTimerTimeout()
{
    for (int i = 0; i < roombas_.size(); ++i)
    {
        int index = selectedRoombas_.indexOf(roombas_.at(i));
        //roomba is selected
        if(roombas_.at(i)->getIcon()->isSelected() ||
                roombas_.at(i)->getStartPoint()->isSelected())
        {
            mainWindow_->setRoombaStatusData(roombas_.at(i));
            //both the roomba and it's startPoint are selected for clarity
            roombas_.at(i)->getIcon()->setSelected(true);
            roombas_.at(i)->getStartPoint()->setSelected(true);
            if (index == -1)  //selected roomba isn't yet in selectedRoombas_
            {
                selectedRoombas_.append(roombas_.at(i));
            }
        }
        else //roomba is not selected
        {
            if (index != -1)  //unselected roomba is in selectedRoombas_
            {
                selectedRoombas_.remove(index);
            }
        }
    }
}

void FleetManager::createRoomba(PoiQGraphicsEllipseItem *startPoint)
{
    Croi::RoombaRoowifi* roomba = new
        Croi::RoombaRoowifi(startPoint, map_, this);
    //Croi::RoombaRoowifi roomba = new Croi::RoombaSerial();
    //TODO: Croi::RoombaRoowifi roomba = new Croi::RoombaVirtual();
    roombas_.append(roomba);
    selectedRoombas_.append(roomba);
}

void FleetManager::addPoi(PoiQGraphicsEllipseItem* poi)
{
    pois_.insert(poi);
}

void FleetManager::addWall(WallQGraphicsLineItem* wall)
{
    walls_.insert(wall);
}

std::set<WallQGraphicsLineItem *> FleetManager::getWalls()
{
    return walls_;
}

std::set<PoiQGraphicsEllipseItem *> FleetManager::getPOIs()
{
    return pois_;
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

void FleetManager::removeAllObjects()
{
    for (std::set<PoiQGraphicsEllipseItem*>::iterator i = pois_.begin();
         i != pois_.end(); ++i)
    {
        map_->scene()->removeItem(*i);
        delete *i;
        pois_.erase(i);
    }
    for (std::set<WallQGraphicsLineItem*>::iterator i = walls_.begin();
         i != walls_.end(); ++i)
    {
        map_->scene()->removeItem(*i);
        delete *i;
        walls_.erase(i);
    }
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

//removes selectedRoombas traces
void FleetManager::removeTraces()
{
    for (int i = 0; i < selectedRoombas_.size(); ++i)
    {
        selectedRoombas_.at(i)->removeTraces();
    }
}

void FleetManager::go2Poi()
{
    QPointF poiCoordinate = (*pois_.begin())->pos();
    selectedRoombas_.at(0)->go2Point(poiCoordinate);

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
        //TODO: check issue #3 on Github, implementation is missing
        //bool isTrace = false;
        //goes through collidingItems and removes POI if it
        //finds an item that isn't a trace
        //while (!collidingItems.empty())
        //{
        //  collidingItems.pop_front();
        //}
    }
}

void FleetManager::connect(std::string stdip)
{
    if (selectedRoombas_.empty())
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please select one Roomba!"));
    }
    else if(selectedRoombas_.size() > 1)
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please select only one Roomba!"));
    }
    else
    {
       selectedRoombas_.at(0)->rmb_connect(stdip);
    }
}

void FleetManager::disconnect()
{
    if (selectedRoombas_.empty())
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please select one Roomba!"));
    }
    else if(selectedRoombas_.size() > 1)
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please select only one Roomba!"));
    }
    else
    {
       selectedRoombas_.at(0)->disconnect();
    }
}

void FleetManager::clean()
{
    if (selectedRoombas_.empty())
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please select a Roomba!"));
    }
    else
    {
        for (int i = 0; i < selectedRoombas_.size(); ++i)
        {
            selectedRoombas_.at(i)->clean();
        }
    }
}

void FleetManager::allMotorsOn()
{
    if (selectedRoombas_.empty())
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please select a Roomba!"));
    }
    else
    {
        for (int i = 0; i < selectedRoombas_.size(); ++i)
        {
            selectedRoombas_.at(i)->allMotorsOn();
        }
    }
}

void FleetManager::allMotorsOff()
{
    if (selectedRoombas_.empty())
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please select a Roomba!"));
    }
    else
    {
        for (int i = 0; i < selectedRoombas_.size(); ++i)
        {
            selectedRoombas_.at(i)->allMotorsOff();
        }
    }
}

void FleetManager::safeMode()
{
    if (selectedRoombas_.empty())
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please select a Roomba!"));
    }
    else
    {
        for (int i = 0; i < selectedRoombas_.size(); ++i)
        {
            selectedRoombas_.at(i)->safeMode();
        }
    }
}

void FleetManager::fullMode()
{
    if (selectedRoombas_.empty())
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please select a Roomba!"));
    }
    else
    {
        for (int i = 0; i < selectedRoombas_.size(); ++i)
        {
            selectedRoombas_.at(i)->fullMode();
        }
    }
}

void FleetManager::resetAngle()
{
    if (selectedRoombas_.empty())
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please select a Roomba!"));
    }
    else
    {
        for (int i = 0; i < selectedRoombas_.size(); ++i)
        {
            selectedRoombas_.at(i)->resetAngle();
        }
    }
}

void FleetManager::playSong(unsigned int songNum)
{
    if (selectedRoombas_.empty())
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please select a Roomba!"));
    }
    else
    {
        for (int i = 0; i < selectedRoombas_.size(); ++i)
        {
            selectedRoombas_.at(i)->playSong(songNum);
        }
    }
}

void FleetManager::setVelocity(int velocity)
{
    if (selectedRoombas_.empty())
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please select a Roomba!"));
    }
    else
    {
        for (int i = 0; i < selectedRoombas_.size(); ++i)
        {
            if (selectedRoombas_.at(i)->getVelocity() != 0)
            {
                selectedRoombas_.at(i)->drive(velocity);
            }
        }
    }
}

void FleetManager::drive( int velocity, int radius )
{
    if (selectedRoombas_.empty())
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please select a Roomba!"));
    }
    else
    {
        for (int i = 0; i < selectedRoombas_.size(); ++i)
        {
            selectedRoombas_.at(i)->drive(velocity, radius);
        }
    }
}

void FleetManager::drive( int velocity)
{
    if (selectedRoombas_.empty())
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please select a Roomba!"));
    }
    else
    {
        for (int i = 0; i < selectedRoombas_.size(); ++i)
        {
            selectedRoombas_.at(i)->drive(velocity);
        }
    }
}

FleetManager::~FleetManager()
{
    for (int i = 0; i < roombas_.size(); ++i)
    {
        roombas_[i]->disconnect();
    }
}
