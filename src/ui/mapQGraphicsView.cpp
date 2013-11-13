#include "mapQGraphicsView.h"
#include <QPointF>
#include <QReadLocker>
#include "poiQGraphicsEllipseItem.h"
#include "wallQGraphicsLineItem.h"
#include "atcQGraphicsRectItem.h"
#include "fleetManager.h"
#include <iostream>
#include "mainwindow.h"
#include <cmath>
#include <QDebug>
#include <QMessageBox>
#include "flogger.h"

MapQGraphicsView::MapQGraphicsView(FleetManager* fleetManager, QWidget* parent) :
    QGraphicsView(parent), wallToBeAddedStartPoint_(NULL), atcToBeAddedStartPoint_(NULL),
    mapScale_(1), traceShown_(true), fleetManager_(fleetManager),
    wallToBeAddedStartPointText_(NULL), wallToBeAddedEndPointText_(NULL),
	atcToBeAddedStartPointText_(NULL), atcToBeAddedEndPointText_(NULL)
{
    setRenderHints(QPainter::Antialiasing);
}

void MapQGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (event->button()==Qt::LeftButton)
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
                    (fleetManager_, p.x(), p.y(), p.x(), p.y());
        wallToBeAddedStartPoint_ = new QPointF(p.x(), p.y());
        scene()->addItem(wallToBeAdded_);
            // Add textual coordinates to the beginning of the wall line
            wallToBeAddedStartPointText_ = new QGraphicsSimpleTextItem("X: " + QString::number(p.x()*Util::COORDCORRECTION) +
                                                                       " Y: " +  QString::number(p.y()*Util::COORDCORRECTION));
            wallToBeAddedStartPointText_->setPos(p);
            wallToBeAddedStartPointText_->setZValue(5);
            QBrush wallToBeAddedStartPointBrush(Qt::GlobalColor::blue);
            wallToBeAddedStartPointText_->setBrush(wallToBeAddedStartPointBrush);
            scene()->addItem(wallToBeAddedStartPointText_);
        qDebug() << "Pos: " << p.x() << "y: "<< p.y();
            (*flog.ts)<< QString("Start a wall @ x: %1 y: %2").arg(p.x()).arg(p.y()) <<endl;
    }
 	else if (selectedPaintTool_ == Util::SelectedPaintTool::ATC)
    {

        setDragMode(QGraphicsView::NoDrag);
        qDebug() << "Draw a atc!";
        atcToBeAdded_ = new AtcQGraphicsRectItem
                (p.x(), p.y(), 0, 0);
//                (0,0,0,0);

        atcToBeAddedStartPoint_ = new QPointF(p.x(), p.y());
        scene()->addItem(atcToBeAdded_);

        // Add textual coordinates to the Top Left corner point of the Rectangle
        atcToBeAddedStartPointText_ = new QGraphicsSimpleTextItem("X: " + QString::number(p.x()) + " Y: " +  QString::number(p.y()));
        atcToBeAddedStartPointText_->setPos(p);
        atcToBeAddedStartPointText_->setZValue(5);
        QBrush atcToBeAddedStartPointBrush(Qt::GlobalColor::blue);
        atcToBeAddedStartPointText_->setBrush(atcToBeAddedStartPointBrush);
        scene()->addItem(atcToBeAddedStartPointText_);

        qDebug() << "Square corner X: " + QString::number(p.x()) + " Y: " +  QString::number(p.y());
//        AtcQGraphicsRectItem* atc = new AtcQGraphicsRectItem
//                (0.0-POIWIDTH/5.0-TRACEWIDTH/2.0, 0.0-POIWIDTH/5.0-TRACEWIDTH/2.0, 5*POIWIDTH, 5*POIWIDTH);

//              (0.0-5*POIWIDTH/2.0, 0.0-5*POIWIDTH/2.0, 5*POIWIDTH, 5*POIWIDTH);
//        (0.0, 0.0, 5*POIWIDTH, 5*POIWIDTH);


//        atc->setPos(p);
//        fleetManager_->pushATC(p);

//        qDebug() << "pushATC(p) p.x(): " << p.x() << "p.y(): "<< p.y();


//        atc->setFlag(QGraphicsItem::ItemIsSelectable,true);
//        atc->setFlag(QGraphicsItem::ItemIsMovable,false); // Disabled so that the mapChanged signal works as expected
//        scene()->addItem(atc);
//        fleetManager_->addAtc(atcToBeAdded_);

//        qDebug() << "Adding scenePos().x(): " << atc->scenePos().x()
//                 << " ,scenePos().y(): " << atc->scenePos().y();
        qDebug() << "squP.x(): " << p.x() << "P.y(): "<< p.y();
        (*flog.ts)<< QString("Draw a ATC, Adding ATC with x: %1 y: %2").arg(p.x()).arg(p.y()) <<endl;

        emit mapChanged();
    }
    else if (selectedPaintTool_ == Util::SelectedPaintTool::POI)
    {
            if(fleetManager_->isBlocked(&p))
            {
                QMessageBox::warning
                        (parentWidget(), "", tr("POI must be inserted further away from wall!"));
            }
            else
            {
        qDebug() << "Draw a poi!";
        setDragMode(QGraphicsView::NoDrag);
        PoiQGraphicsEllipseItem* poi = new PoiQGraphicsEllipseItem
                        (fleetManager_, 0.0-Util::POIWIDTH/2.0, 0.0-Util::POIWIDTH/2.0, Util::POIWIDTH, Util::POIWIDTH);
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
        }
        else if (selectedPaintTool_ == Util::SelectedPaintTool::START ||
                 selectedPaintTool_ == Util::SelectedPaintTool::STARTVIRTUAL)
    {
        qDebug() << "Draw a start!";
            (*flog.ts) << "Draw a start" << endl;
        setDragMode(QGraphicsView::NoDrag);
        PoiQGraphicsEllipseItem *startPoint = new PoiQGraphicsEllipseItem
                    (fleetManager_, 0.0-Util::POIWIDTH*2.0/3.0, 0.0-Util::POIWIDTH*2.0/3.0,
                     Util::POIWIDTH*4.0/3.0, Util::POIWIDTH*4.0/3.0);
        startPoint->setPos(p);
        QBrush brush(Qt::GlobalColor::green);
        startPoint->setBrush(brush);
        startPoint->setFlag(QGraphicsItem::ItemIsSelectable,true);
        //movable needs additional logic before allowing
        startPoint->setFlag(QGraphicsItem::ItemIsMovable,false);
        //TODO: Add deleleting of startPoint when moving it
        scene()->addItem(startPoint);
            if(selectedPaintTool_ == Util::SelectedPaintTool::START)
            {
                fleetManager_->createRoomba(startPoint, false); //real roomba
            }
            else
            {
                fleetManager_->createRoomba(startPoint, true); //virtual roomba
            }
        }
    }
    // Call the base class implementation to deliver the event for QGraphicsScene
    QGraphicsView::mousePressEvent(event);
}

void MapQGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    // event->button() returns always Qt::NoButton for mouse move events, so button check is not needed
    if (wallToBeAddedStartPoint_ != NULL)
    {
        QPointF p = mapToScene(event->pos());
        if (selectedPaintTool_ == Util::SelectedPaintTool::WALL)
        {
            wallToBeAdded_->setLine(wallToBeAddedStartPoint_->x(), wallToBeAddedStartPoint_->y(),
                                    p.x(), p.y());
        }
        // Add textual coordinates to the end of the wall line
        if (wallToBeAddedEndPointText_ == NULL)
        {
            wallToBeAddedEndPointText_ = new QGraphicsSimpleTextItem("X: " + QString::number(p.x()) + " Y: " +  QString::number(p.y()));
            wallToBeAddedEndPointText_->setPos(p);
            QBrush wallToBeAddedEndPointTextBrush(Qt::GlobalColor::blue);
            wallToBeAddedEndPointText_->setBrush(wallToBeAddedEndPointTextBrush);
            scene()->addItem(wallToBeAddedEndPointText_);
            wallToBeAddedEndPointText_->setZValue(5);
        }
        // Update textual coordinates in the end of the wall line
        else
        {
            // Calculate the current wall length
            float deltaX = wallToBeAdded_->line().x2() - wallToBeAdded_->line().x1();
            float deltaY = wallToBeAdded_->line().y1() - wallToBeAdded_->line().y2();
            // TODO: Add pythagoras from here and iRoomba to some utility function
            float distance = sqrt(pow(deltaX,2)+pow(deltaY,2) )*Util::COORDCORRECTION;
            // Use offset to avoid colliding with cursor
            QPointF pointToDrawLength = p;
            pointToDrawLength.setY(pointToDrawLength.y()+Util::WALLLENGTHINDICATOROFFSET);
            wallToBeAddedEndPointText_->setPos(pointToDrawLength);
            wallToBeAddedEndPointText_->setText(QString::number(distance, 'f', 0) + "cm"); // Ignore decimals in wall length
        }
    }
    if (atcToBeAddedStartPoint_ != NULL)
    {
        QPointF p = mapToScene(event->pos());
        if (selectedPaintTool_ == Util::SelectedPaintTool::ATC)
        {
            atcToBeAdded_->setRect(atcToBeAddedStartPoint_->x(), atcToBeAddedStartPoint_->y(),
                         p.x()-atcToBeAddedStartPoint_->x(), p.y()-atcToBeAddedStartPoint_->y());

            scene()->update();

        }
        // Add textual coordinates to the end of the rectangle
        if (atcToBeAddedEndPointText_ == NULL)
        {
            atcToBeAddedEndPointText_ = new QGraphicsSimpleTextItem("X: " + QString::number(p.x()) + " Y: " +  QString::number(p.y()));
            atcToBeAddedEndPointText_->setPos(p);
            QBrush atcToBeAddedEndPointTextBrush(Qt::GlobalColor::blue);
            atcToBeAddedEndPointText_->setBrush(atcToBeAddedEndPointTextBrush);
            scene()->addItem(atcToBeAddedEndPointText_);
            atcToBeAddedEndPointText_->setZValue(5);
        }
        // Update textual coordinates in the end of the rectangle
        // line width is 3, we have to add 3 for p.x() and p.y()

        else
        {

            atcToBeAddedEndPointText_->setPos(p);
            atcToBeAddedEndPointText_->setText("X: " + QString::number(p.x()) + " Y: " +  QString::number(p.y())
                                               + "\nW " + QString::number(abs(atcToBeAdded_->rect().width())+3)
                                              + " H " + QString::number(abs(atcToBeAdded_->rect().height())+3)  );
        }
    }
    // Call the base class implementation to deliver the event for QGraphicsScene
    QGraphicsView::mouseMoveEvent(event);
}

void MapQGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button()==Qt::LeftButton)
    {
    	if (selectedPaintTool_ == Util::SelectedPaintTool::WALL)
  	  	{
  	      wallToBeAdded_->setFlag(QGraphicsItem::ItemIsSelectable,true);
  	      wallToBeAdded_->setFlag(QGraphicsItem::ItemIsMovable,false); // Disabled so that the mapChanged signal works as expected
  	      fleetManager_->addWall(wallToBeAdded_);
  	          if(fleetManager_->removeBlockedPois())  //POIs blocked by the wall are removed
  	          {
  	              QMessageBox::warning
  	                      (parentWidget(), "", tr("POIs too close to the wall were removed"));
  	          }
  	          wallToBeAdded_ = NULL;
  		      delete wallToBeAddedStartPoint_;
  		      wallToBeAddedStartPoint_ = NULL;
  	          delete wallToBeAddedStartPointText_;
  	          wallToBeAddedStartPointText_ = NULL;
  	          delete wallToBeAddedEndPointText_;
  	          wallToBeAddedEndPointText_ = NULL;
  		      emit mapChanged();
  	  	}
    	else if (selectedPaintTool_ == Util::SelectedPaintTool::ATC)
    	{
    	    atcToBeAdded_->setFlag(QGraphicsItem::ItemIsSelectable,true);
    	    atcToBeAdded_->setFlag(QGraphicsItem::ItemIsMovable,false); // Disabled so that the mapChanged signal works as expected
    	    fleetManager_->addAtc(atcToBeAdded_);
    	    delete atcToBeAddedStartPoint_;
    	    atcToBeAddedStartPoint_ = NULL;
    	    delete atcToBeAddedStartPointText_;
    	    atcToBeAddedStartPointText_ = NULL;
    	    delete atcToBeAddedEndPointText_;
    	    atcToBeAddedEndPointText_ = NULL;
    	    emit mapChanged();
    	}
	}
    // Call the base class implementation to deliver the event for QGraphicsScene
    QGraphicsView::mouseReleaseEvent(event);
}

void MapQGraphicsView::setSelectedPaintTool(Util::SelectedPaintTool tool)
{
    selectedPaintTool_ = tool;
}

//gives map's width in mm
unsigned int MapQGraphicsView::getmapScale()
{
    return mapScale_;
}

//give new width in mm.
void MapQGraphicsView::setmapScale(double scaleFactor)
{
    mapScale_ = scaleFactor;
    resetTransform();
    //MAP'S WIDTH IN PIXELS IS FIXED ATM
    scale(scaleFactor, scaleFactor);
}

MapQGraphicsView::~MapQGraphicsView()
{ 
}
