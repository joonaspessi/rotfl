#include "mapQGraphicsView.h"
#include <QPointF>
#include <QReadLocker>
#include "poiQGraphicsEllipseItem.h"
#include "wallQGraphicsLineItem.h"
#include "fleetManager.h"
#include <iostream>
#include "mainwindow.h"
#include <cmath>
#include <QDebug>

MapQGraphicsView::MapQGraphicsView(FleetManager* fleetManager, QWidget* parent) :
    QGraphicsView(parent), wallToBeAddedStartPoint_(NULL),
    mapWidth_(398), traceShown_(true), fleetManager_(fleetManager)
{
    setRenderHints(QPainter::Antialiasing);
}

void MapQGraphicsView::mousePressEvent(QMouseEvent *event)
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
        wallToBeAdded_ = new WallQGraphicsLineItem
                (p.x(), p.y(), p.x(), p.y());
        wallToBeAddedStartPoint_ = new QPointF(p.x(), p.y());
        scene()->addItem(wallToBeAdded_);
        qDebug() << "Pos: " << p.x() << "y: "<< p.y();
    }
    else if (selectedPaintTool_ == Util::SelectedPaintTool::POI)
    {
        qDebug() << "Draw a poi!";
        setDragMode(QGraphicsView::NoDrag);
        PoiQGraphicsEllipseItem* poi = new PoiQGraphicsEllipseItem
                (0.0-POIWIDTH/2.0, 0.0-POIWIDTH/2.0, POIWIDTH, POIWIDTH);
        poi->setPos(p);
        poi->setFlag(QGraphicsItem::ItemIsSelectable,true);
        poi->setFlag(QGraphicsItem::ItemIsMovable,true);
        scene()->addItem(poi);
        fleetManager_->addPoi(poi);
        qDebug() << "Adding POI with x: " << poi->scenePos().x()
                 << " , y: " << poi->scenePos().y();
    }
    else if (selectedPaintTool_ == Util::SelectedPaintTool::START)
    {
        qDebug() << "Draw a start!";
        setDragMode(QGraphicsView::NoDrag);
        PoiQGraphicsEllipseItem *startPoint = new PoiQGraphicsEllipseItem
                (0.0-POIWIDTH*2.0/3.0, 0.0-POIWIDTH*2.0/3.0,
                 POIWIDTH*4.0/3.0, POIWIDTH*4.0/3.0);
        startPoint->setPos(p);
        QBrush brush(Qt::GlobalColor::green);
        startPoint->setBrush(brush);
        startPoint->setFlag(QGraphicsItem::ItemIsSelectable,true);
        //movable needs additional logic before allowing
        startPoint->setFlag(QGraphicsItem::ItemIsMovable,false);
        //TODO: Add deleleting of startPoint when moving it
        scene()->addItem(startPoint);
        MainWindow* mainwindow = qobject_cast<MainWindow*>(parent());
        fleetManager_->createRoomba(startPoint);
    }
    // Call the base class implementation to deliver the event for QGraphicsScene
    QGraphicsView::mousePressEvent(event);
}

void MapQGraphicsView::mouseMoveEvent(QMouseEvent *event)
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

void MapQGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (selectedPaintTool_ == Util::SelectedPaintTool::WALL)
    {
        wallToBeAdded_->setFlag(QGraphicsItem::ItemIsSelectable,true);
        wallToBeAdded_->setFlag(QGraphicsItem::ItemIsMovable,true);
        fleetManager_->addWall(wallToBeAdded_);
        delete wallToBeAddedStartPoint_;
        wallToBeAddedStartPoint_ = NULL;
    }
    // Call the base class implementation to deliver the event for QGraphicsScene
    QGraphicsView::mouseReleaseEvent(event);
}

void MapQGraphicsView::setSelectedPaintTool(Util::SelectedPaintTool tool)
{
    selectedPaintTool_ = tool;
}

//updates all roomba's location, heading and trace in the map.
void MapQGraphicsView::updateLoc(QVector<Croi::IRoomba *>* roombas)
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
        QPointF first(roombas->at(i)->getLoc().x()+cos(angle)*ARROWWIDTH,
                      roombas->at(i)->getLoc().y()+sin(angle)*ARROWWIDTH);
        points.append(first);
        double tempAngle = angle+40.0*PI/180.0;
        points.append(QPointF(roombas->at(i)->getLoc().x()
                              -cos(tempAngle)*ARROWWIDTH,
                              roombas->at(i)->getLoc().y()
                              -sin(tempAngle)*ARROWWIDTH));
        tempAngle -= 80.0*PI/180.0;
        points.append(QPointF(roombas->at(i)->getLoc().x()
                              -cos(tempAngle)*ARROWWIDTH,
                              roombas->at(i)->getLoc().y()
                              -sin(tempAngle)*ARROWWIDTH));
        points.append(first);
        QPolygonF triangle(points);
        //calculate the point at the back of the triangle
        double triangleX = (points.at(1).x()+points.at(2).x())/2.0;
        double triangleY = (points.at(1).y()+points.at(2).y())/2.0;

        //ROOMBA'S ICON WILL REPLACE THIS IMPLEMENTATION
        //AND IT WILL BE PLACED IN IROOMBA CLASS AND HERE IT WILL
        //BE SIMPLY ADDED TO THE SCENE
        QGraphicsPolygonItem* polygon = roombas->at(i)->getPolygon();
        QGraphicsLineItem* curSpeed = NULL;

        if (polygon == NULL)  //first update
        {
            polygon = scene()->addPolygon(triangle);
            //color of the roombaTriangle is blue
            QBrush triangleBrush(Qt::GlobalColor::blue);
            polygon->setBrush(triangleBrush);
            roombas->at(i)->setPolygon(polygon);
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
            curSpeed = roombas->at(i)->getCurSpeed();
            polygon->setPolygon(triangle);
            curSpeed->setLine(triangleX, triangleY,
                              triangleX-cos(angle)*ARROWWIDTH*2.0*speed,
                              triangleY-sin(angle)*ARROWWIDTH*2.0*speed);
            curSpeed->setZValue(1);
        }
        polygon->setZValue(1);
    }
    fleetManager_->checkPoiCollision();
}

//gives map's width in mm
int MapQGraphicsView::giveMapWidth()
{
    return mapWidth_;
}

//give new width in mm.
void MapQGraphicsView::setMapWidth(int width)
{
    mapWidth_ = width;
    resetTransform();
    //MAP'S WIDTH IN PIXELS IS FIXED ATM
    scale(398.0/mapWidth_, 398.0/mapWidth_);
}

MapQGraphicsView::~MapQGraphicsView()
{ 
}
