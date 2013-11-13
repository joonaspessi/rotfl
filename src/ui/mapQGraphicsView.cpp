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
#include "flogger.h"

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
        (*flog.ts) << "Draw a cursor!" << endl;
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
        (*flog.ts)<< QString("Start a wall @ x: %1 y: %2").arg(p.x()).arg(p.y()) <<endl;
    }
    else if (selectedPaintTool_ == Util::SelectedPaintTool::POI)
    {
        qDebug() << "Draw a poi!";
        setDragMode(QGraphicsView::NoDrag);
        PoiQGraphicsEllipseItem* poi = new PoiQGraphicsEllipseItem
                (0.0-POIWIDTH/2.0, 0.0-POIWIDTH/2.0, POIWIDTH, POIWIDTH);
        poi->setPos(p);
        poi->setFlag(QGraphicsItem::ItemIsSelectable,true);
        poi->setFlag(QGraphicsItem::ItemIsMovable,false); // Disabled so that the mapChanged signal works as expected
        scene()->addItem(poi);
        fleetManager_->addPoi(poi);
        qDebug() << "Adding POI with x: " << poi->scenePos().x()
                 << " , y: " << poi->scenePos().y();
        emit mapChanged();
        (*flog.ts)<< QString("Draw a POI, Adding POI with x: %1 y: %2").arg(p.x()).arg(p.y()) <<endl;
    }
    else if (selectedPaintTool_ == Util::SelectedPaintTool::START)
    {
        qDebug() << "Draw a start!";
        (*flog.ts) << "Draw a start" << endl;
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
        wallToBeAdded_->setFlag(QGraphicsItem::ItemIsMovable,false); // Disabled so that the mapChanged signal works as expected
        fleetManager_->addWall(wallToBeAdded_);
        delete wallToBeAddedStartPoint_;
        wallToBeAddedStartPoint_ = NULL;
        emit mapChanged();
    }
    // Call the base class implementation to deliver the event for QGraphicsScene
    QGraphicsView::mouseReleaseEvent(event);
}

void MapQGraphicsView::setSelectedPaintTool(Util::SelectedPaintTool tool)
{
    selectedPaintTool_ = tool;
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
