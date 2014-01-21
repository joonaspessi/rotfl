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
    QObject(parent), mainWindow_(mainWindow), map_(NULL), go2PoisOn_(false),
    cleaningAtcsOn_(false)
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

    //creating vertices (with all values set to zero)
    QVector<Util::Vertice*> verticeRow;
    for(int i = 0; i < int(ceil(double(Util::PIXELMAPWIDTH)/Util::VERTICEWIDTH)); ++i)
    {
        for(int j = 0; j < int(ceil(double(Util::PIXELMAPWIDTH)/Util::VERTICEWIDTH)); ++j)
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
    for(unsigned int i = 1; i < vertices_.at(0).size()-1; ++i)
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
    for(unsigned int i = 1; i < vertices_.at(0).size()-1; ++i)
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
        for(unsigned int j = 1; j < vertices_.at(0).size()-1; ++j)
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
        for(unsigned int j = 0; j < vertices_.at(0).size(); ++j)
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
    QObject::connect(roomba, SIGNAL(areaCleaned()), this, SLOT(clean()));
}

void FleetManager::addPoi(PoiQGraphicsEllipseItem* poi)
{
    pois_.append(poi);
}

void FleetManager::addAtc(AtcQGraphicsRectItem* atc)
{
    atcs_.insert(atc);
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

                    //calculates how many vertices have to be blocked that are next to the vertice that has a wall
                    //includes some leeway (the factor 1.5), byt that is lessened by flooring
                    int lInit = static_cast<int>(i-floor(double(Util::REALROOMBAWIDTH)/2*1.5/Util::REALVERTICEWIDTH));
                    int mInit = static_cast<int>(j-floor(double(Util::REALROOMBAWIDTH)/2*1.5/Util::REALVERTICEWIDTH));
                    int lEnd = static_cast<int>(i+floor(double(Util::REALROOMBAWIDTH)/2*1.5/Util::REALVERTICEWIDTH));
                    int mEnd = static_cast<int>(j+floor(double(Util::REALROOMBAWIDTH)/2*1.5/Util::REALVERTICEWIDTH));
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

QVector<PoiQGraphicsEllipseItem *> FleetManager::getPOIs()
{
    return pois_;
}

std::set<AtcQGraphicsRectItem *> FleetManager::getATCs()
{
    return atcs_;
}

void FleetManager::removeRedObjects()
{
    for (int i = 0; i < pois_.size(); ++i)
    {
        if (pois_.at(i)->pen().color() == Qt::GlobalColor::red)
        {
            map_->scene()->removeItem(pois_.at(i));
            delete pois_.at(i);
            pois_.remove(i);
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

	for (std::set<AtcQGraphicsRectItem*>::iterator i = atcs_.begin();
         i != atcs_.end(); ++i)
    {
        if ((*i)->pen().color() == Qt::GlobalColor::red)
        {
            map_->scene()->removeItem(*i);
            delete *i;
            atcs_.erase(i);
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
    for (QVector<PoiQGraphicsEllipseItem*>::iterator i = pois_.begin();
         i != pois_.end(); ++i)
    {
        removePoi(*i);
    }
    for (std::set<AtcQGraphicsRectItem*>::iterator i = atcs_.begin();
         i != atcs_.end(); ++i)
    {
        map_->scene()->removeItem(*i);
        delete *i;
        atcs_.erase(i);
    }
    for (std::set<WallQGraphicsLineItem*>::iterator i = walls_.begin();
         i != walls_.end(); ++i)
    {
        removeWall(*i);
    }
}

void FleetManager::removePoi(PoiQGraphicsEllipseItem* poi)
{
    int index = pois_.indexOf(poi);
    if(index == -1)
    {
        return;
    }
    map_->scene()->removeItem(poi);
    pois_.remove(index);
    delete poi;
}

void FleetManager::removeAtc(AtcQGraphicsRectItem* atc)
{
    map_->scene()->removeItem(atc);
    atcs_.erase(atc);
    delete atc;
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
    if(go2PoisOn_)
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Old go2Pois command is still active!"));
        return;
    }

    if(pois_.empty())
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please make a POI!"));
    }
    else if(roombas_.empty())
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please make a Roomba!"));
    }
    else
    {
        managedRoombas_ = roombas_;

        for (int i = 0; i < pois_.size() && !managedRoombas_.empty(); ++i)
        {
            if(!go2Poi(pois_.at(i)))
            {
                if(i == 0)
                {
                    QMessageBox::warning
                            (mainWindow_, "", tr("No selected Roomba is ready"));
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
            --i;    //removing makes new roomba to be at i
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
        poi->setGettingCollected(true);
        go2PoisOn_ = true;
        selectedRoomba->usePath();  //selectedRoomba will go to Poi
        return true;
    }
}

void FleetManager::stopFleet(bool wallHit)
{
    go2PoisOn_ = false;
    for(unsigned int i = 0; i < roombas_.size(); ++i)
    {
        roombas_.at(i)->stop();
    }

    if(wallHit)
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Roomba has hit an unidentified object. Fleet management operation terminated."));
    }
}

void FleetManager::poiCollected(Croi::IRoomba* collector, PoiQGraphicsEllipseItem *poi)
{
    removePoi(poi);
    if(pois_.empty())
    {
        QMessageBox::information
        (mainWindow_, "", tr("All points collected!"));
        go2PoisOn_ = false;
        return;
    }

    //find the next nearest poi
    double newDistance = std::numeric_limits<double>::max();
    double bestDistance = std::numeric_limits<double>::max();
    PoiQGraphicsEllipseItem* nearestPoi = NULL;
    for(QVector<PoiQGraphicsEllipseItem*>::iterator i = pois_.begin();
         i != pois_.end(); ++i)
    {
        if(!(*i)->getGettingCollected())
        {
            newDistance = collector->calcPath(vertices_, *i);
            //if poi is unreachable to this roomba
            if(newDistance < bestDistance)
            {
                nearestPoi = *i;
               //old path must be ignored
                //the best path will be recalculated. This is not very
                //stylish here (works nicer in go2poi)
            }
            collector->ignorePath();

        }
    }

    if(nearestPoi != NULL) //
    {
        collector->calcPath(vertices_, nearestPoi);  //recalculating best path
        collector->getDestPoi()->setGettingCollected(true);
        collector->usePath();
        return;
    }

    //if this code is executed, pois_ isn't empty and there is a poi unreachable by
    //one of the roombas. If all roombas have stopped (their destPoi is NULL), then
    //there exists an unreachable poi
    bool unreachableExists = true;
    for(int i = 0; i < roombas_.size(); ++i)
    {
        if(roombas_.at(i)->getDestPoi() != NULL)
        {
            unreachableExists = false;
        }
    }
    if(unreachableExists)
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Rest of the points are unreachable!"));
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
    for (unsigned int i = 0; i < pois_.size(); ++i)
    {
        QPointF point = pois_.at(i)->pos();
        if (isBlocked(&point))
        {
            blockedPoiFound = true;
            removePoi(pois_.at(i));
            --i; //this fixes the indexing change
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

void FleetManager::goDock()
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
            // TODO: Add correct dock position here
            //selectedRoombas_.at(i)->go2Point(QPointF(50,50));
            selectedRoombas_.at(i)->goDock();
        }
    }
}
void FleetManager::clean()
{

    if (roombas_.empty())
    {
        QMessageBox::warning
        (mainWindow_, "", tr("Please connect a Roomba!"));
    }
    else
    {
        for (int i = 0; i < roombas_.size(); ++i)
        {
            if(MoveRobotToNearestArea(i))
            {
                cleaningAtcsOn_ = true;
                roombas_.at(i)->clean();
            }
            else
            {
                if(cleaningAtcsOn_)
                {
//                   cleaningAtcsOn_ = false;
//                    QMessageBox::information
//                        (mainWindow_, "", tr("All areas cleaned"));
                }
                else
                {
                    QMessageBox::warning
                    (mainWindow_, "", tr("No areas to clean!"));
                }
                return;
            }
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
// TODO: Check to work like go2POIs
int FleetManager::findNearestPoint(QPointF roombaPos)
{
    float shortestdistance = 99999.99f;
    int val = -1;
    int i = 0;
    for (std::set<AtcQGraphicsRectItem*>::iterator ii=atcs_.begin(); ii != atcs_.end();++ii)
    {
        AtcQGraphicsRectItem * p = *ii;
//        if (p->isGettingCleaned())
//        {
//            std::set<AtcQGraphicsRectItem*>::iterator testI = ii;
//            ++testI;
//            if(testI == atcs_.end())
//            {
//                return -1;
//            }
//            continue;
//        }
        QPointF point = p->boundingRect().topLeft();
        float deltaX = point.x()-roombaPos.x();
        float deltaY = roombaPos.y() - point.y();
        float distance = sqrt(pow(deltaX,2)+pow(deltaY,2) );
        if (distance < shortestdistance) { shortestdistance = distance; val =i; }
        ++i;
    }

    qDebug() << "val is: " << val;


    return val;
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

// TODO: Check if this is similar to Juhani's logic!
bool FleetManager::MoveRobotToNearestArea(int j) {
    QPointF roombaPos = roombas_.at(j)->getLoc();
    int i = findNearestPoint(roombaPos);
    if (i==-1) { return false; }
    // go2Point currently works only with selected robot

    //QPointF actPos=ATC2Start_.at(i).atcPos;
    std::set<AtcQGraphicsRectItem*>::const_iterator ii = atcs_.begin();
    std::advance(ii,i);
    //AtcQGraphicsRectItem *ik; ik->boundingRect().height()
    (*ii)->setGettingCleaned();
    roombas_.at(j)->setDestAtc(*ii);
    QPointF actPos = (*ii)->boundingRect().topLeft();

    roombas_.at(j)->setSquare((*ii)->boundingRect().width(),(*ii)->boundingRect().height());

    qDebug() << "actPos X: " + QString::number(actPos.rx()) + " actPos Y: " +  QString::number(actPos.ry());

    //these functions return a reference
    actPos.ry() += (Util::PIXELCLEANWIDTH+3)/2;
    actPos.rx() += (Util::PIXELCLEANWIDTH+3)/2;

    qDebug() << "Square actual start X: actPos.rx() += CTRACEWIDTH/2" + QString::number(actPos.rx()) + " Y: " +  QString::number(actPos.ry());



    roombas_.at(j)->go2Point(actPos);
    //ATC2Start_.remove(i);
//    map_->scene()->removeItem(*ii);
//    delete *ii;
    atcs_.erase(ii);
    return true;
}


FleetManager::~FleetManager()
{
    for (int i = 0; i < roombas_.size(); ++i)
    {
        roombas_[i]->disconnect();
    }
}
