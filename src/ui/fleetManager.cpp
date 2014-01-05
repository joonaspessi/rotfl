#include "mainwindow.h"
#include "fleetManager.h"
#include "croi/roombaRoowifi.h"
#include <QDebug>
#include <QObject>
#include <QList>
#include <QMessageBox>
#include "uiUtils.h"
#include <limits>
#include "croi/roombaVirtual.h"

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
        for(unsigned int j = 0; j < mapW; ++j)
        {
            verticeRow.append(new Util::Vertice());
        }
        vertices_.append(verticeRow);
        verticeRow.clear();
    }
    //setting corner vertices. note: .at(x).at(y)
    //setting vertice at 0,0
    vertices_.at(0).at(0)->e = vertices_.at(1).at(0);
    vertices_.at(0).at(0)->se = vertices_.at(1).at(1);
    vertices_.at(0).at(0)->s = vertices_.at(0).at(1);
    //setting vertice at last,0
    vertices_.at(vertices_.size()-1).at(0)->topLeftX = Util::VERTICEWIDTH*(vertices_.size()-1);
    vertices_.at(vertices_.size()-1).at(0)->pos.setX(Util::VERTICEWIDTH*(vertices_.size()-1)+Util::VERTICEWIDTH/2.0);
    vertices_.at(vertices_.size()-1).at(0)->s = vertices_.at(vertices_.size()-1).at(1);
    vertices_.at(vertices_.size()-1).at(0)->sw = vertices_.at(vertices_.size()-2).at(1);
    vertices_.at(vertices_.size()-1).at(0)->w = vertices_.at(vertices_.size()-2).at(0);
    //setting vertice at 0,last
    vertices_.at(0).at(vertices_.size()-1)->topLeftY = Util::VERTICEWIDTH*(vertices_.size()-1);
    vertices_.at(0).at(vertices_.size()-1)->pos.setY(Util::VERTICEWIDTH*(vertices_.size()-1)+Util::VERTICEWIDTH/2.0);
    vertices_.at(0).at(vertices_.size()-1)->n = vertices_.at(0).at(vertices_.size()-2);
    vertices_.at(0).at(vertices_.size()-1)->ne = vertices_.at(1).at(vertices_.size()-2);
    vertices_.at(0).at(vertices_.size()-1)->e = vertices_.at(1).at(vertices_.size()-1);
    //setting vertice at last,last
    vertices_.at(vertices_.size()-1).at(vertices_.size()-1)->topLeftX = Util::VERTICEWIDTH*(vertices_.size()-1);
    vertices_.at(vertices_.size()-1).at(vertices_.size()-1)->topLeftY = Util::VERTICEWIDTH*(vertices_.size()-1);
    vertices_.at(vertices_.size()-1).at(vertices_.size()-1)->pos.setX(Util::VERTICEWIDTH*(vertices_.size()-1)+Util::VERTICEWIDTH/2.0);
    vertices_.at(vertices_.size()-1).at(vertices_.size()-1)->pos.setY(Util::VERTICEWIDTH*(vertices_.size()-1)+Util::VERTICEWIDTH/2.0);
    vertices_.at(vertices_.size()-1).at(vertices_.size()-1)->w = vertices_.at(vertices_.size()-2).at(vertices_.size()-1);
    vertices_.at(vertices_.size()-1).at(vertices_.size()-1)->nw = vertices_.at(vertices_.size()-2).at(vertices_.size()-2);
    vertices_.at(vertices_.size()-1).at(vertices_.size()-1)->n = vertices_.at(vertices_.size()-1).at(vertices_.size()-2);

    //setting vertices from 1,0 to last-1,0
    for(unsigned int i = 1; i < vertices_.size()-1; ++i)
    {
        vertices_.at(i).at(0)->topLeftX = Util::VERTICEWIDTH*i;
        vertices_.at(i).at(0)->pos.setX(Util::VERTICEWIDTH*i+Util::VERTICEWIDTH/2.0);
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
        vertices_.at(i).at(vertices_.size()-1)->pos.setX(Util::VERTICEWIDTH*i+Util::VERTICEWIDTH/2.0);
        vertices_.at(i).at(vertices_.size()-1)->pos.setY(Util::VERTICEWIDTH*(vertices_.size()-1)+Util::VERTICEWIDTH/2.0);
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
        vertices_.at(0).at(i)->pos.setY(Util::VERTICEWIDTH*i+Util::VERTICEWIDTH/2.0);
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
        vertices_.at(vertices_.size()-1).at(i)->pos.setX(Util::VERTICEWIDTH*(vertices_.size()-1)+Util::VERTICEWIDTH/2.0);
        vertices_.at(vertices_.size()-1).at(i)->pos.setY(Util::VERTICEWIDTH*i+Util::VERTICEWIDTH/2.0);
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
            vertices_.at(i).at(j)->pos.setX(Util::VERTICEWIDTH*i+Util::VERTICEWIDTH/2.0);
            vertices_.at(i).at(j)->pos.setY(Util::VERTICEWIDTH*j+Util::VERTICEWIDTH/2.0);
            vertices_.at(i).at(j)->n = vertices_.at(i).at(j-1);
            vertices_.at(i).at(j)->ne = vertices_.at(i+1).at(j-1);
            vertices_.at(i).at(j)->e = vertices_.at(i+1).at(j);
            vertices_.at(i).at(j)->se = vertices_.at(i+1).at(j+1);
            vertices_.at(i).at(j)->s = vertices_.at(i).at(j+1);
            vertices_.at(i).at(j)->sw = vertices_.at(i-1).at(j+1);
            vertices_.at(i).at(j)->w = vertices_.at(i-1).at(j);
            vertices_.at(i).at(j)->nw = vertices_.at(i-1).at(j-1);
        }
    }

    //drawing all vertices
    QPen pen(Qt::GlobalColor::gray);
    pen.setWidth(1);
    for(unsigned int i = 0; i < vertices_.size(); ++i)
    {
        QVector<Util::Vertice*> verticeRow;
        for(unsigned int j = 0; j < vertices_.size(); ++j)
        {
            map_->scene()->addLine(vertices_.at(i).at(j)->topLeftX, vertices_.at(i).at(j)->topLeftY,
                                   vertices_.at(i).at(j)->topLeftX+Util::VERTICEWIDTH-1,
                                   vertices_.at(i).at(j)->topLeftY, pen);
            map_->scene()->addLine(vertices_.at(i).at(j)->topLeftX, vertices_.at(i).at(j)->topLeftY,
                                   vertices_.at(i).at(j)->topLeftX,
                                   vertices_.at(i).at(j)->topLeftY+Util::VERTICEWIDTH-1, pen);
            map_->scene()->addLine(vertices_.at(i).at(j)->topLeftX+Util::VERTICEWIDTH-1,
                                   vertices_.at(i).at(j)->topLeftY,
                                   vertices_.at(i).at(j)->topLeftX+Util::VERTICEWIDTH-1,
                                   vertices_.at(i).at(j)->topLeftY+Util::VERTICEWIDTH-1, pen);
            map_->scene()->addLine(vertices_.at(i).at(j)->topLeftX,
                                   vertices_.at(i).at(j)->topLeftY+Util::VERTICEWIDTH-1,
                                   vertices_.at(i).at(j)->topLeftX+Util::VERTICEWIDTH-1,
                                   vertices_.at(i).at(j)->topLeftY+Util::VERTICEWIDTH-1, pen);
        }
    }


//    //DEBUG DRAWING FOR SHOWING THE CONNECTIONS FROM VERTICE at a,b
//    int a = 1;
//    int b = 1;
//    map_->scene()->addLine(vertices_.at(a).at(b)->pos.x(),vertices_.at(a).at(b)->pos.y(),
//                           vertices_.at(a).at(b)->n->pos.x(),vertices_.at(a).at(b)->n->pos.y());
//    map_->scene()->addLine(vertices_.at(a).at(b)->pos.x(),vertices_.at(a).at(b)->pos.y(),
//                           vertices_.at(a).at(b)->ne->pos.x(),vertices_.at(a).at(b)->ne->pos.y());
//    map_->scene()->addLine(vertices_.at(a).at(b)->pos.x(),vertices_.at(a).at(b)->pos.y(),
//                           vertices_.at(a).at(b)->e->pos.x(),vertices_.at(a).at(b)->e->pos.y());
//    map_->scene()->addLine(vertices_.at(a).at(b)->pos.x(),vertices_.at(a).at(b)->pos.y(),
//                           vertices_.at(a).at(b)->se->pos.x(),vertices_.at(a).at(b)->se->pos.y());
//    map_->scene()->addLine(vertices_.at(a).at(b)->pos.x(),vertices_.at(a).at(b)->pos.y(),
//                           vertices_.at(a).at(b)->s->pos.x(),vertices_.at(a).at(b)->s->pos.y());
//    map_->scene()->addLine(vertices_.at(a).at(b)->pos.x(),vertices_.at(a).at(b)->pos.y(),
//                           vertices_.at(a).at(b)->sw->pos.x(),vertices_.at(a).at(b)->sw->pos.y());
//    map_->scene()->addLine(vertices_.at(a).at(b)->pos.x(),vertices_.at(a).at(b)->pos.y(),
//                           vertices_.at(a).at(b)->w->pos.x(),vertices_.at(a).at(b)->w->pos.y());
//    map_->scene()->addLine(vertices_.at(a).at(b)->pos.x(),vertices_.at(a).at(b)->pos.y(),
//                           vertices_.at(a).at(b)->nw->pos.x(),vertices_.at(a).at(b)->nw->pos.y());

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

void FleetManager::createRoomba(PoiQGraphicsEllipseItem *startPoint, bool virtualRoomba)
{
    Croi::IRoomba *roomba;
    if(virtualRoomba)
    {
        roomba = new Croi::RoombaVirtual(startPoint, map_, this);
    }
    else
    {
        roomba = new Croi::RoombaRoowifi(startPoint, map_, this);
    }
    //Croi::RoombaSerial* roomba = new Croi::RoombaSerial(startPoint, map_, this);

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

    //breaking verticeconnections to vertices where there are
    //walls
    for(unsigned int i = 0; i < vertices_.size(); ++i)
    {
        for(unsigned int j = 0; j < vertices_.at(0).size(); ++j)
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
                    vertices_.at(i).at(j)->hasWall = true;

                    int lInit = static_cast<int>(i-ceil(1.0/Util::VERTICEWIDTH*20.0));
                    int mInit = static_cast<int>(j-ceil(1.0/Util::VERTICEWIDTH*20.0));
                    int lEnd = static_cast<int>(i+ceil(1.0/Util::VERTICEWIDTH*20.0));
                    int mEnd = static_cast<int>(j+ceil(1.0/Util::VERTICEWIDTH*20.0));
                    if(lInit < 0)
                    {
                        lInit = 0;
                    }
                    if (mInit < 0)
                    {
                        mInit = 0;
                    }
                    if(lEnd > vertices_.size()-1)
                    {
                        lEnd = vertices_.size()-1;
                    }
                    if (mEnd > vertices_.at(0).size()-1)
                    {
                        mEnd = vertices_.at(0).size()-1;
                    }
                    for(int l = lInit; l <= lEnd; ++l)
                    {
                        for(int m = mInit; m <= mEnd ; ++m)
                        {
                            vertices_.at(l).at(m)->blocked = true;
                        }
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
    for(unsigned int i = 0; i < vertices_.size(); ++i)
    {
        for(unsigned int j = 0; j < vertices_.at(0).size(); ++j)
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
                vertices_.at(i).at(j)->hasWall = false;
            }

        }
    }

    map_->scene()->removeItem(wall);
    delete wall;
    walls_.erase(wall);

    for(unsigned int i = 0; i < vertices_.size(); ++i)
    {
        for(unsigned int j = 0; j < vertices_.at(0).size(); ++j)
        {
            //unblocked already so checks can be skipped
            if(!vertices_.at(i).at(j)->blocked)
            {
                continue;
            }

            bool wallNear = false;
            int lInit = static_cast<int>(i-ceil(1.0/Util::VERTICEWIDTH*20.0));
            int mInit = static_cast<int>(j-ceil(1.0/Util::VERTICEWIDTH*20.0));
            int lEnd = static_cast<int>(i+ceil(1.0/Util::VERTICEWIDTH*20.0));
            int mEnd = static_cast<int>(j+ceil(1.0/Util::VERTICEWIDTH*20.0));
            if(lInit < 0)
            {
                lInit = 0;
            }
            if (mInit < 0)
            {
                mInit = 0;
            }
            if(lEnd > vertices_.size()-1)
            {
                lEnd = vertices_.size()-1;
            }
            if (mEnd > vertices_.at(0).size()-1)
            {
                mEnd = vertices_.at(0).size()-1;
            }
            for(int l = lInit; l <= lEnd; ++l)
            {
                for(int m = mInit; m <= mEnd ; ++m)
                {
                    if(vertices_.at(l).at(m)->hasWall == true)
                    {
                        wallNear = true;
                    }
                }
            }

            if(!wallNear)
            {
                vertices_.at(i).at(j)->blocked = false;
            }
        }
    }
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

void FleetManager::go2Pois()
{
    if (pois_.empty())
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please make a POI!"));
    }
    else if (selectedRoombas_.empty())
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please select a Roomba!"));
    }
    else
    {
        managedRoombas_ = selectedRoombas_;

        for (std::set<PoiQGraphicsEllipseItem*>::iterator i = pois_.begin();
             i != pois_.end(); ++i)
        {
            if(!go2Poi(*i))
            {
                if(i == pois_.begin())
                {
                    QMessageBox::warning
                            (mainWindow_, "", tr("No selected Roomba is ready"));
                }
                else
                {
                    QMessageBox::warning
                            (mainWindow_, "", tr("TODO: Roombas to have a task list for collecting unlimited amount of POIs"));
                }

                return;
            }
        }
    }
}

bool FleetManager::go2Poi(PoiQGraphicsEllipseItem *poi)
{
    double distance = std::numeric_limits<double>::max();
    double compareDistance = 0.0;
    bool isReadyFound = false;
    Croi::IRoomba* selectedRoomba = NULL;

    //nearest IRoomba will be selectedRoomba
    for(unsigned int i = 0; i < managedRoombas_.size(); ++i)
    {
        //path calculated only for ready IRoombas
        if(managedRoombas_.at(i)->isReady())
        {
            isReadyFound = true;
            compareDistance = managedRoombas_.at(i)
                    ->calcPath(vertices_, poi);
            if(compareDistance < distance)
            {
                selectedRoomba = managedRoombas_.at(i);
                distance = compareDistance;
            }
        }
    }
    //the paths of other IRoombas are ignored and
    //the selectedRoomba is taken away from managedRoombas_
    for(unsigned int i = 0; i < managedRoombas_.size(); ++i)
    {
        if(managedRoombas_.at(i) == selectedRoomba)
        {
            managedRoombas_.remove(i);
        }
        else
        {
            managedRoombas_.at(i)->ignorePath();
        }
    }


    if(!isReadyFound) //if none of selectedRoombas_ is ready
    {
        return false;
    }
    //if no route found
    else if(selectedRoomba == NULL)
    {
        QMessageBox::warning
                (mainWindow_, "", tr("No possible route to one of the POIs!"));
        return true;
    }
    else
    {
        selectedRoomba->usePath();  //selectedRoomba will go to Poi
        return true;
    }
}

void FleetManager::stopFleet()
{
    for(unsigned int i = 0; i < roombas_.size(); ++i)
    {
        roombas_.at(i)->stop();
    }
}

bool FleetManager::isBlocked(QPointF* point)
{
    for(unsigned int i = 0; i < vertices_.size(); ++i)
    {
        for(unsigned int j = 0; j < vertices_.at(0).size(); ++j)
        {
            if(vertices_.at(i).at(j)->topLeftX <= point->x() &&
               vertices_.at(i).at(j)->topLeftX+Util::VERTICEWIDTH-1 >= point->x() &&
               vertices_.at(i).at(j)->topLeftY <= point->y() &&
               vertices_.at(i).at(j)->topLeftY+Util::VERTICEWIDTH-1 >= point->y())
            {
                return vertices_.at(i).at(j)->blocked;
            }
        }
    }
}

bool FleetManager::removeBlockedPois()
{
    bool blockedPoiFound = false;
    for (std::set<PoiQGraphicsEllipseItem*>::iterator i = pois_.begin();
         i != pois_.end(); ++i)
    {
        QPointF point = (*i)->pos();
        if (isBlocked(&point))
        {
            blockedPoiFound = true;
            removePoi(*i);
        }
    }
    return blockedPoiFound;
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

void FleetManager::correctLocation(Util::Direction direction)
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
        for (int i = 0; i < selectedRoombas_.size(); ++i)
        {
            selectedRoombas_.at(i)->correctLocation(direction);
        }
    }
}

void FleetManager::correctAngle(bool clockWise)
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
        for (int i = 0; i < selectedRoombas_.size(); ++i)
        {
            selectedRoombas_.at(i)->correctAngle(clockWise);
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
