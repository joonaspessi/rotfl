#include <QPointF>
#include <QReadLocker>
#include "poiQGraphicsEllipseItem.h"
#include "wallQGraphicsLineItem.h"
#include <set>
#include <iostream>
#include "mainwindow.h"
#include "mapQGraphicsView.h"
#include <cmath>
#include <QDebug>

mapQGraphicsView::mapQGraphicsView(QWidget* parent) :
    QGraphicsView(parent), wallToBeAddedStartPoint_(NULL),
    mapWidth_(398), traceShown_(true),
	selectedRoomba_(NULL)
{
    setRenderHints(QPainter::Antialiasing);
}

void mapQGraphicsView::removePoi(poiQGraphicsEllipseItem* poi)
{
    scene()->removeItem(poi);
    pois_.erase(poi);
    delete poi;
}

void mapQGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QPointF p = mapToScene(event->pos());
    if (selectedPaintTool_ == Util::SelectedPaintTool::CURSOR)
    {
        setDragMode(QGraphicsView::RubberBandDrag);
        qDebug() << "Draw a cursor!";
    }
    else if (selectedPaintTool_ == Util::SelectedPaintTool::WALL)
    {
        setDragMode(QGraphicsView::NoDrag);
        qDebug() << "Start a wall!";
        wallToBeAdded_ = new wallQGraphicsLineItem
                (p.x(), p.y(), p.x(), p.y());
        wallToBeAddedStartPoint_ = new QPointF(p.x(), p.y());
        scene()->addItem(wallToBeAdded_);
        qDebug() << "Pos: " << p.x() << "y: "<< p.y();
    }
    else if (selectedPaintTool_ == Util::SelectedPaintTool::POI)
    {
        qDebug() << "Draw a poi!";
        setDragMode(QGraphicsView::NoDrag);
        poiQGraphicsEllipseItem* poi = new poiQGraphicsEllipseItem
                (0.0-POIWIDTH/2.0, 0.0-POIWIDTH/2.0, POIWIDTH, POIWIDTH);
        poi->setPos(p);
        poi->setFlag(QGraphicsItem::ItemIsSelectable,true);
        poi->setFlag(QGraphicsItem::ItemIsMovable,true);
        scene()->addItem(poi);
        pois_.insert(poi);
        qDebug() << "Adding POI with x: " << poi->scenePos().x()
                 << " , y: " << poi->scenePos().y();
    }
    else if (selectedPaintTool_ == Util::SelectedPaintTool::START)// && selectedRoomba_->getStartPoint() == NULL)
    {
        qDebug() << "Draw a start!";
        setDragMode(QGraphicsView::NoDrag);
        poiQGraphicsEllipseItem *startPoint = new poiQGraphicsEllipseItem
                (p.x()-POIWIDTH*2.0/3.0, p.y()-POIWIDTH*2.0/3.0,
                 POIWIDTH*4.0/3.0, POIWIDTH*4.0/3.0);
        //startPoint->setPos();
        QBrush brush(Qt::GlobalColor::green);
        startPoint->setBrush(brush);
        startPoint->setFlag(QGraphicsItem::ItemIsSelectable,true);
        startPoint->setFlag(QGraphicsItem::ItemIsMovable,true);
        scene()->addItem(startPoint);
    }
    // Call the base class implementation to deliver the event for QGraphicsScene
    QGraphicsView::mousePressEvent(event);
}

void mapQGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (wallToBeAddedStartPoint_ != NULL)
    {
        QPointF p = mapToScene(event->pos());
        if (selectedPaintTool_ == Util::SelectedPaintTool::WALL)
        {
            wallToBeAdded_->setLine(wallToBeAddedStartPoint_->x(), wallToBeAddedStartPoint_->y(),
                                    p.x(), p.y());
        }
    }
    // Call the base class implementation to deliver the event for QGraphicsScene
    QGraphicsView::mouseMoveEvent(event);
}

void mapQGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (selectedPaintTool_ == Util::SelectedPaintTool::WALL)
    {
        wallToBeAdded_->setFlag(QGraphicsItem::ItemIsSelectable,true);
        wallToBeAdded_->setFlag(QGraphicsItem::ItemIsMovable,true);
        walls_.insert(wallToBeAdded_);
        delete wallToBeAddedStartPoint_;
        wallToBeAddedStartPoint_ = NULL;
    }
    // Call the base class implementation to deliver the event for QGraphicsScene
    QGraphicsView::mouseReleaseEvent(event);
}

void mapQGraphicsView::setSelectedPaintTool(Util::SelectedPaintTool tool)
{
    selectedPaintTool_ = tool;
}

// TODO: OLD CODE LEFT TEMPORARILY REFERENCE. TO BE REMOVED
//void mapQGraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
//{
//    QPointF p = mapToScene(event->pos());
//    qDebug() << "x: " << p.x() << "y: " << p.y();
//    if (event->button() == Qt::LeftButton)
//    {
//        if(selectedRoomba_->getStartPoint() == NULL)
//        {
//            startPoint_ = new poiQGraphicsEllipseItem
//                    (p.x()-POIWIDTH*2.0/3.0, p.y()-POIWIDTH*2.0/3.0,
//                     POIWIDTH*4.0/3.0, POIWIDTH*4.0/3.0);
//            //startPoint_->setPos();
//            QBrush brush(Qt::GlobalColor::green);
//            startPoint_->setBrush(brush);
//            scene()->addItem(startPoint_);
//        }
//        else
//        {
//            poiQGraphicsEllipseItem* poi = new poiQGraphicsEllipseItem
//                    (0.0-POIWIDTH/2.0, 0.0-POIWIDTH/2.0, POIWIDTH, POIWIDTH);
//            poi->setPos(p);
//            scene()->addItem(poi);
//            pois_.insert(poi);
//            qDebug() << "Adding POI with x: " << poi->scenePos().x()
//                     << " , y: " << poi->scenePos().y();
//        }
//    }
//    else if (event->button() == Qt::RightButton)
//    {
//        if (wallStartPoint_ == NULL)
//        {
//            wallStartPoint_ = new QGraphicsRectItem(0, 0, 2, 2);
//            wallStartPoint_->setPos(p);
//            scene()->addItem(wallStartPoint_);
//        }
//        else
//        {
//            wallQGraphicsLineItem* wall = new wallQGraphicsLineItem
//                    (wallStartPoint_->x(), wallStartPoint_->y(), p.x(), p.y());
//            walls_.insert(wall);
//            scene()->addItem(wall);
//            scene()->removeItem(wallStartPoint_);
//            delete wallStartPoint_;
//            wallStartPoint_ = NULL;
//        }
//    }
//    checkPoiCollision();
//}

void mapQGraphicsView::removeRedObjects()
{
    for (std::set<poiQGraphicsEllipseItem*>::iterator i = pois_.begin();
        i != pois_.end(); ++i)
    {
        if ((*i)->pen().color() == Qt::GlobalColor::red)
        {
            scene()->removeItem(*i);
            delete *i;
            pois_.erase(i);
        }
    }
    for (std::set<wallQGraphicsLineItem*>::iterator i = walls_.begin();
        i != walls_.end(); ++i)
    {
        if ((*i)->pen().color() == Qt::GlobalColor::red)
        {
            scene()->removeItem(*i);
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

void mapQGraphicsView::ifShowTraces(QVector<Croi::IRoomba*>* roombas)
{
    for (unsigned int i = 0; i < roombas->size(); ++i)
    {
        roombas->at(i)->ifShowTraces();
    }
}

void mapQGraphicsView::removeTraces(QVector<Croi::IRoomba*>* roombas)
        {
    for (unsigned int i = 0; i < roombas->size(); ++i)
{
        QVector<QGraphicsLineItem*>* traces = roombas->at(i)->getTraces();
        for (unsigned int i = 0; i < traces->size(); ++i)
    {
            scene()->removeItem(traces->at(i));
    }
        roombas->at(i)->removeTraces();
    }
}

//updates roomba's location and heading. Keeps a trace
//Remember following special radiuses
//Straight = 32768 or 32767
//Turn in place clockwise = -1
//Turn in place counter-clockwise = 1
void mapQGraphicsView::updateLoc(QVector<Croi::IRoomba *>* roombas)
{
    for(unsigned int i = 0; i < roombas->size(); ++i)
    {
        //startPoint_ needs to be specified on map
        if (roombas->at(i)->getStartPoint()== NULL)
        {
            continue;
    }
    //normalized speed
        double speed = static_cast<double>(roombas->at(i)->getVelocity())/500.0;

    if(speed < 0)
    {
        speed *= -1;
    }

    //making the correctly angled roombaTriangle
        //THIS WILL BE REPLACED WITH ROOMBA ICON
        double angle = static_cast<double>(roombas->at(i)->getCurrentAngle());
    QVector<QPointF> points;
        QPointF first(roombas->at(i)->getRoombasLocation().x()+cos(angle)*ARROWWIDTH,
                      roombas->at(i)->getRoombasLocation().y()+sin(angle)*ARROWWIDTH);
    points.append(first);
        double tempAngle = angle+40.0*PI/180.0;
        points.append(QPointF(roombas->at(i)->getRoombasLocation().x()
                              -cos(tempAngle)*ARROWWIDTH,
                              roombas->at(i)->getRoombasLocation().y()
                              -sin(tempAngle)*ARROWWIDTH));
    tempAngle -= 80.0*PI/180.0;
        points.append(QPointF(roombas->at(i)->getRoombasLocation().x()
                              -cos(tempAngle)*ARROWWIDTH,
                              roombas->at(i)->getRoombasLocation().y()
                              -sin(tempAngle)*ARROWWIDTH));
    points.append(first);
    QPolygonF triangle(points);
    //calculate the point at the back of the triangle
    double triangleX = (points.at(1).x()+points.at(2).x())/2.0;
    double triangleY = (points.at(1).y()+points.at(2).y())/2.0;

        QVector<QGraphicsLineItem*>* traces = roombas->at(i)->getTraces();

        //adds all new tracelines (usually just 1 but this supports calling
        //updateLoc less frequently
        for (unsigned int i = traces->size()-1; i >= 0; --i)
        {
            if (scene()->items().contains(traces->at(i)))
            {
                break; //all new tracelines added
            }
            else
            {
    if (!traceShown_)
    {
                    traces->at(i)->setVisible(false);
                }
                scene()->addItem(traces->at(i));
            }
    }

        //ROOMBA'S ICON WILL REPLACE THIS IMPLEMENTATION
        //AND IT WILL BE PLACED IN IROOMBA CLASS AND HERE IT WILL
        //BE SIMPLY ADDED TO THE SCENE
        QGraphicsPolygonItem* curPoint = roombas->at(i)->getCurPoint();
        QGraphicsLineItem* curSpeed = roombas->at(i)->getCurSpeed();
        if (curPoint == NULL)  //first update
    {
            curPoint = scene()->addPolygon(triangle);
        //color of the roombaTriangle is blue
        QBrush triangleBrush(Qt::GlobalColor::blue);
            curPoint->setBrush(triangleBrush);
            roombas->at(i)->setCurPoint(curPoint);
        QPen curSpeedPen(Qt::GlobalColor::blue);
        curSpeedPen.setWidth(TRACEWIDTH/4.0);
            //don't you dare say that this here piece of code is not beautiful!
            roombas->at(i)->setCurSpeed(scene()->addLine(triangleX, triangleY,
                                         triangleX-cos(angle)*ARROWWIDTH*2.0*speed,
                                         triangleY-sin(angle)*ARROWWIDTH*2.0*speed,
                                         curSpeedPen));
    }
    else
    {
            curPoint->setPolygon(triangle);
            curSpeed->setLine(triangleX, triangleY,
                               triangleX-cos(angle)*ARROWWIDTH*2.0*speed,
                               triangleY-sin(angle)*ARROWWIDTH*2.0*speed);
    }

        curPoint->setZValue(1);
        curSpeed->setZValue(1);

    }
    checkPoiCollision();
}

//gives map's width in mm
int mapQGraphicsView::giveMapWidth()
{
    return mapWidth_;
}

//give new width in mm.
void mapQGraphicsView::setMapWidth(int width)
{
    mapWidth_ = width;
    resetTransform();
    //MAP'S WIDTH IN PIXELS IS FIXED ATM
    scale(398.0/mapWidth_, 398.0/mapWidth_);
}

QPointF mapQGraphicsView::getNextPoi()
{
    std::set<poiQGraphicsEllipseItem*>::iterator i = pois_.begin();
    if (i == pois_.end())
    {
        QPointF point(0, 0);
        return point;
    }
    //x = (*i)->pos().x();
    //y = (*i)->pos().y();
    QPointF point = (*i)->pos();
    return point;

}


void mapQGraphicsView::checkPoiCollision()
{
    for (std::set<poiQGraphicsEllipseItem*>::iterator i = pois_.begin();
        i != pois_.end(); ++i)
    {
        if (!(*i)->collidingItems().empty())
        {
            removePoi(*i);
        }
    }
}

mapQGraphicsView::~mapQGraphicsView()
{ 
//    removeTraces();

//    for (std::set<wallQGraphicsLineItem*>::iterator i = walls_.begin();
//        i != walls_.end(); ++i)
//    {
//        scene()->removeItem(*i);
//        delete *i;
//    }

//    for (std::set<poiQGraphicsEllipseItem*>::iterator i = pois_.begin();
//        i != pois_.end(); ++i)
//    {
//        scene()->removeItem(*i);
//        delete *i;
//    }
}
