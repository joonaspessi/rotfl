#include "mainwindow.h"
#include "fleetManager.h"
#include "croi/roombaRoowifi.h"
#include <QDebug>
#include <QObject>
#include <QList>
#include <QMessageBox>
#include "uiUtils.h"

FleetManager::FleetManager(MainWindow* mainWindow, QObject *parent):
    QObject(parent), mainWindow_(mainWindow), map_(NULL)
{

    updateTimer_ = new QTimer(this);
    QObject::connect(updateTimer_, SIGNAL(timeout()), this, SLOT(updateTimerTimeout()));
    updateTimer_->setSingleShot(false);
    updateTimer_->start(100);
}


//map_ cannot be set in constructor because map_ is constructed after
//FleetManager. vertices are also set.
//TODO: possibly own function for setting vertices and change construction
//order of the map_ and FleetManager
void FleetManager::setMap(MapQGraphicsView* map)
{
    map_ = map;
    //map's width in vertices
    unsigned int mapW = map_->getMapWidth()/Util::VERTICEWIDTH;

    //creating vertices (with all values set to zero)
    QVector<Util::Vertice*> verticeRow;
    for(unsigned int i = 0; i < mapW; ++i)
    {
        verticeRow.append(new Util::Vertice());
    }
    for(unsigned int i = 0; i < mapW; ++i)
    {
        vertices_.append(verticeRow);
    }

    //setting corner vertices. note: .at(x).at(y)
    //setting vertice at 0,0
    vertices_.at(0).at(0)->e = vertices_.at(1).at(0);
    vertices_.at(0).at(0)->se = vertices_.at(1).at(1);
    vertices_.at(0).at(0)->s = vertices_.at(0).at(1);
    //setting vertice at last,0
    vertices_.at(vertices_.size()-1).at(0)->topLeftX = Util::VERTICEWIDTH*(vertices_.size()-1);
    vertices_.at(vertices_.size()-1).at(0)->s = vertices_.at(vertices_.size()-1).at(1);
    vertices_.at(vertices_.size()-1).at(0)->sw = vertices_.at(vertices_.size()-2).at(1);
    vertices_.at(vertices_.size()-1).at(0)->w = vertices_.at(vertices_.size()-2).at(0);
    //setting vertice at 0,last
    vertices_.at(0).at(vertices_.size()-1)->topLeftY = Util::VERTICEWIDTH*(vertices_.size()-1);
    vertices_.at(0).at(vertices_.size()-1)->n = vertices_.at(vertices_.size()-2).at(0);
    vertices_.at(0).at(vertices_.size()-1)->ne = vertices_.at(vertices_.size()-2).at(1);
    vertices_.at(0).at(vertices_.size()-1)->e = vertices_.at(vertices_.size()-1).at(1);
    //setting vertice at last,last
    vertices_.at(vertices_.size()-1).at(vertices_.size()-1)->topLeftX = Util::VERTICEWIDTH*(vertices_.size()-1);
    vertices_.at(vertices_.size()-1).at(vertices_.size()-1)->topLeftY = Util::VERTICEWIDTH*(vertices_.size()-1);
    vertices_.at(vertices_.size()-1).at(vertices_.size()-1)->w = vertices_.at(vertices_.size()-2).at(vertices_.size()-1);
    vertices_.at(vertices_.size()-1).at(vertices_.size()-1)->nw = vertices_.at(vertices_.size()-2).at(vertices_.size()-2);
    vertices_.at(vertices_.size()-1).at(vertices_.size()-1)->n = vertices_.at(vertices_.size()-1).at(vertices_.size()-2);
    //setting vertices from 1,0 to last-1,0
    for(unsigned int i = 1; i < vertices_.size()-1; ++i)
    {
        vertices_.at(i).at(0)->topLeftX = Util::VERTICEWIDTH*i;
        vertices_.at(i).at(0)->e = vertices_.at(i+1).at(0);
        vertices_.at(i).at(0)->se = vertices_.at(i+1).at(1);
        vertices_.at(i).at(0)->s = vertices_.at(i).at(1);
        vertices_.at(i).at(0)->sw = vertices_.at(i-1).at(1);
        vertices_.at(i).at(0)->w = vertices_.at(i-1).at(0);
    }
    //setting vertices from 1,last to last-1,last
    for(unsigned int i = 1; i < vertices_.size()-1; ++i)
    {
        vertices_.at(i).at(vertices_.size()-1)->topLeftX = Util::VERTICEWIDTH*i;
        vertices_.at(i).at(vertices_.size()-1)->topLeftY = Util::VERTICEWIDTH*(vertices_.size()-1);
        vertices_.at(i).at(vertices_.size()-1)->w = vertices_.at(i-1).at(vertices_.size()-1);
        vertices_.at(i).at(vertices_.size()-1)->nw = vertices_.at(i-1).at(vertices_.size()-2);
        vertices_.at(i).at(vertices_.size()-1)->n = vertices_.at(i).at(vertices_.size()-2);
        vertices_.at(i).at(vertices_.size()-1)->ne = vertices_.at(i+1).at(vertices_.size()-2);
        vertices_.at(i).at(vertices_.size()-1)->e = vertices_.at(i+1).at(vertices_.size()-1);
    }
    //setting vertices from 0,1 to 0,last-1
    for(unsigned int i = 1; i < vertices_.size()-1; ++i)
    {
        vertices_.at(0).at(i)->topLeftY = Util::VERTICEWIDTH*i;
        vertices_.at(0).at(i)->n = vertices_.at(0).at(i-1);
        vertices_.at(0).at(i)->ne = vertices_.at(1).at(i-1);
        vertices_.at(0).at(i)->e = vertices_.at(1).at(i);
        vertices_.at(0).at(i)->se = vertices_.at(1).at(i+1);
        vertices_.at(0).at(i)->s = vertices_.at(0).at(i+1);
    }
    //setting vertices from last,1 to last,last-1
    for(unsigned int i = 1; i < vertices_.size()-1; ++i)
    {
        vertices_.at(vertices_.size()-1).at(i)->topLeftX = Util::VERTICEWIDTH*(vertices_.size()-1);
        vertices_.at(vertices_.size()-1).at(i)->topLeftY = Util::VERTICEWIDTH*i;
        vertices_.at(vertices_.size()-1).at(i)->s = vertices_.at(vertices_.size()-1).at(i+1);
        vertices_.at(vertices_.size()-1).at(i)->sw = vertices_.at(vertices_.size()-2).at(i+1);
        vertices_.at(vertices_.size()-1).at(i)->w = vertices_.at(vertices_.size()-2).at(i);
        vertices_.at(vertices_.size()-1).at(i)->nw = vertices_.at(vertices_.size()-2).at(i-1);
        vertices_.at(vertices_.size()-1).at(i)->n = vertices_.at(vertices_.size()-1).at(i-1);
    }

    //setting the rest of vertices.
    //they are the non-border ones from 1,1 to last-1,last-1.
    for(unsigned int i = 1; i < vertices_.size()-1; ++i)
    {
        QVector<Util::Vertice*> verticeRow;
        for(unsigned int j = 1; j < vertices_.size()-1; ++j)
        {
            vertices_.at(i).at(j)->topLeftX = Util::VERTICEWIDTH*i;
            vertices_.at(i).at(j)->topLeftY = Util::VERTICEWIDTH*j;
            vertices_.at(i).at(j)->n = vertices_.at(i).at(i-1);
            vertices_.at(i).at(j)->ne = vertices_.at(i+1).at(i-1);
            vertices_.at(i).at(j)->e = vertices_.at(i+1).at(i);
            vertices_.at(i).at(j)->se = vertices_.at(i+1).at(i+1);
            vertices_.at(i).at(j)->s = vertices_.at(i).at(i+1);
            vertices_.at(i).at(j)->sw = vertices_.at(i-1).at(i-1);
            vertices_.at(i).at(j)->w = vertices_.at(i-1).at(i);
            vertices_.at(i).at(j)->nw = vertices_.at(i-1).at(i-1);
        }
    }
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
    Croi::RoombaRoowifi* roomba = new Croi::RoombaRoowifi(startPoint, map_, this);
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

    //breaking verticeconnections
    for(unsigned int i = 1; i < vertices_.size(); ++i)
    {
        for(unsigned int j = 1; j < vertices_.size(); ++j)
        {
            QList<QGraphicsItem*> items =
                map_->items(vertices_.at(i).at(j)->topLeftX,
                            vertices_.at(i).at(j)->topLeftY,
                            Util::VERTICEWIDTH, Util::VERTICEWIDTH);
            for(QList<QGraphicsItem*>::Iterator k = items.begin();
                k != items.end(); ++k)
            {
                if(*k == wall)
                {
                    if(vertices_.at(i).at(j)->n != NULL)
                    {
                        vertices_.at(i).at(j)->n->s = NULL;
                    }
                    if(vertices_.at(i).at(j)->ne != NULL)
                    {
                        vertices_.at(i).at(j)->ne->sw = NULL;
                    }
                    if(vertices_.at(i).at(j)->e != NULL)
                    {
                        vertices_.at(i).at(j)->e->w = NULL;
                    }
                    if(vertices_.at(i).at(j)->se != NULL)
                    {
                        vertices_.at(i).at(j)->se->nw = NULL;
                    }
                    if(vertices_.at(i).at(j)->s != NULL)
                    {
                        vertices_.at(i).at(j)->s->n = NULL;
                    }
                    if(vertices_.at(i).at(j)->sw != NULL)
                    {
                        vertices_.at(i).at(j)->sw->ne = NULL;
                    }
                    if(vertices_.at(i).at(j)->w != NULL)
                    {
                        vertices_.at(i).at(j)->w->e = NULL;
                    }
                    if(vertices_.at(i).at(j)->nw != NULL)
                    {
                        vertices_.at(i).at(j)->nw->se = NULL;
                    }
                    break; //no need to go further through items
                }
            }
        }
    }
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
            removePoi(*i);
        }
    }
    for (std::set<WallQGraphicsLineItem*>::iterator i = walls_.begin();
         i != walls_.end(); ++i)
    {
        if ((*i)->pen().color() == Qt::GlobalColor::red)
        {
            removeWall(*i);
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
        removePoi(*i);
    }
    for (std::set<WallQGraphicsLineItem*>::iterator i = walls_.begin();
         i != walls_.end(); ++i)
    {
        removeWall(*i);
    }
}

void FleetManager::removePoi(PoiQGraphicsEllipseItem* poi)
{
    map_->scene()->removeItem(poi);
    pois_.erase(poi);
    delete poi;
}

void FleetManager::removeWall(WallQGraphicsLineItem* wall)
{
    //renewing verticeconnections
    for(unsigned int i = 1; i < vertices_.size(); ++i)
    {
        for(unsigned int j = 1; j < vertices_.size(); ++j)
        {
            QList<QGraphicsItem*> items =
                map_->items(vertices_.at(i).at(j)->topLeftX,
                            vertices_.at(i).at(j)->topLeftY,
                            Util::VERTICEWIDTH, Util::VERTICEWIDTH);

            //to renew verticeconnections vertice must have the wall
            //we're removing but no other wall inside its area
            bool wallFound = false;
            bool anotherWallFound = false;
            for(QList<QGraphicsItem*>::Iterator k = items.begin();
                k != items.end(); ++k)
            { 
                if(*k == wall)
                {
                    wallFound = true;
                }
                else if((*k)->type() == Util::WALLTYPE)
                {
                    anotherWallFound = true;
                    break;
                }
            }

            if(wallFound && !anotherWallFound)
            {
                if(vertices_.at(i).at(j)->n != NULL)
                {
                    vertices_.at(i).at(j)->n->s = vertices_.at(i).at(j);
                }
                if(vertices_.at(i).at(j)->ne != NULL)
                {
                    vertices_.at(i).at(j)->ne->sw = vertices_.at(i).at(j);
                }
                if(vertices_.at(i).at(j)->e != NULL)
                {
                    vertices_.at(i).at(j)->e->w = vertices_.at(i).at(j);
                }
                if(vertices_.at(i).at(j)->se != NULL)
                {
                    vertices_.at(i).at(j)->se->nw = vertices_.at(i).at(j);
                }
                if(vertices_.at(i).at(j)->s != NULL)
                {
                    vertices_.at(i).at(j)->s->n = vertices_.at(i).at(j);
                }
                if(vertices_.at(i).at(j)->sw != NULL)
                {
                    vertices_.at(i).at(j)->sw->ne = vertices_.at(i).at(j);
                }
                if(vertices_.at(i).at(j)->w != NULL)
                {
                    vertices_.at(i).at(j)->w->e = vertices_.at(i).at(j);
                }
                if(vertices_.at(i).at(j)->nw != NULL)
                {
                    vertices_.at(i).at(j)->nw->se = vertices_.at(i).at(j);
                }
            }
        }
    }

    map_->scene()->removeItem(wall);
    delete wall;
    walls_.erase(wall);
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
