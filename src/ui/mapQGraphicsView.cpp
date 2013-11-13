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
    QGraphicsView(parent), wallStartPoint_(NULL), startPoint_(NULL),
    curPoint_(NULL), curSpeed_(NULL), initX_(0.0), initY_(0.0), angle_(0.0),
    mapWidth_(398), traceShown_(true)
{
    setRenderHints(QPainter::Antialiasing);
}

void mapQGraphicsView::removePoi(poiQGraphicsEllipseItem* poi)
{
    scene()->removeItem(poi);
    pois_.erase(poi);
    delete poi;
}

void mapQGraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QPointF p = mapToScene(event->pos());
    qDebug() << "x: " << p.x() << "y: " << p.y();

    if (event->button() == Qt::LeftButton)
    {
        if(startPoint_ == NULL)
        {
            initX_ = p.x();
            initY_ = p.y();
            QPixmap pixmap;
            pixmap.load("./pics/Roomba_round_icon_small.png");
            QGraphicsPixmapItem * item = scene()->addPixmap(pixmap);
            //item->setPos(p);
            item->setPos(initX_ - pixmap.width()/2, initY_ - pixmap.height()/2);
            qDebug() << item->rotation();
            //item->setRotation(90);
            //qDebug() << item->rotation();

            //QGraphicsPixmapItem item(QPixmap("Roomba_round_icon_small.png"));
            //scene()->addItem(&item);

            /*startPoint_ = new poiQGraphicsEllipseItem
                    (p.x()-POIWIDTH*2.0/3.0, p.y()-POIWIDTH*2.0/3.0,
                     POIWIDTH*4.0/3.0, POIWIDTH*4.0/3.0);
            //startPoint_->setPos();
            QBrush brush(Qt::GlobalColor::green);
            startPoint_->setBrush(brush);
            scene()->addItem(startPoint_);*/

        }
        else
        {
            poiQGraphicsEllipseItem* poi = new poiQGraphicsEllipseItem
                    (0.0-POIWIDTH/2.0, 0.0-POIWIDTH/2.0, POIWIDTH, POIWIDTH);
            poi->setPos(p);
            scene()->addItem(poi);
            pois_.insert(poi);
            qDebug() << "Adding POI with x: " << poi->scenePos().x()
                     << " , y: " << poi->scenePos().y();
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if (wallStartPoint_ == NULL)
        {
            wallStartPoint_ = new QGraphicsRectItem(0, 0, 2, 2);
            wallStartPoint_->setPos(p);
            scene()->addItem(wallStartPoint_);
        }
        else
        {
            wallQGraphicsLineItem* wall = new wallQGraphicsLineItem
                    (wallStartPoint_->x(), wallStartPoint_->y(), p.x(), p.y());
            walls_.insert(wall);
            scene()->addItem(wall);
            scene()->removeItem(wallStartPoint_);
            delete wallStartPoint_;
            wallStartPoint_ = NULL;
        }
    }
    checkPoiCollision();
}

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

    //all tracking and tracing is taken away if startPoint_ is removed
    if (startPoint_ != NULL && startPoint_->pen().color() == Qt::GlobalColor::red)
    {
        delete startPoint_;
        startPoint_ = NULL;
        removeTraces();
        scene()->removeItem(curPoint_);
        delete curPoint_;
        curPoint_ = NULL;
        scene()->removeItem(curSpeed_);
        delete curSpeed_;
        curSpeed_ = NULL;
        initX_= 0.0;
        initY_= 0.0;

    }
}

void mapQGraphicsView::ifShowTraces()
{
    if (traceShown_)
    {
        traceShown_ = false;

        for (QVector<QGraphicsLineItem*>::iterator i = traces_.begin();
            i != traces_.end(); ++i)
        {
            (*i)->setVisible(false);
        }
    }
    else
    {
        traceShown_ = true;

        for (QVector<QGraphicsLineItem*>::iterator i = traces_.begin();
            i != traces_.end(); ++i)
        {
            (*i)->setVisible(true);
        }
    }
}

void mapQGraphicsView::removeTraces()
{
    for (QVector<QGraphicsLineItem*>::iterator i = traces_.begin();
        i != traces_.end(); ++i)
    {
        scene()->removeItem(*i);
        delete *i;
    }
    traces_.clear();
}

//updates roomba's location and heading. Keeps a trace
//Remember following special radiuses
//Straight = 32768 or 32767
//Turn in place clockwise = -1
//Turn in place counter-clockwise = 1
void mapQGraphicsView::updateLoc(int distance, int angle, int radius, int velocity)
{
    if (startPoint_ == NULL) //startPoint_ needs to be specified on map
    {
        return;
    }
    //normalized speed
    double speed = static_cast<double>(velocity)/500.0;

    if(speed < 0)
    {
        speed *= -1;
    }

    //angle for distance calculation
    double angleForDist = angle_-static_cast<double>(angle)*PI*ANGLECORRECTION/180.0;
    //distance changed to cm
    double dist = -static_cast<double>(distance)/10.0*DISTANCECORRECTION;
    //special radiuses mean no adaptation needed
    if (radius != 32768 && radius != 32767 && radius != -1 && radius != 1)
    {
        //corrected distance (and change to cm)
        dist = -2.0*(static_cast<double>(radius))*
                sin(static_cast<double>(distance)/radius/2)/10.0*DISTANCECORRECTION;
        //corrected angle in radians for distance calculation
        //angleForDist = static_cast<double>(angle)*PI/180.0*ANGLECORRECTION/2.0;
        //other version that doesn't work curently
        angleForDist = angle_-static_cast<double>(distance)/radius/2.0;
    }
    //real angle (always used for roomba's angle)
    angle_ -= static_cast<double>(angle)*PI*ANGLECORRECTION/180.0;

    //coordinates are updated
    double x = initX_+cos(angleForDist)*dist;
    double y = initY_+sin(angleForDist)*dist;


    //making the correctly angled roombaTriangle
    QVector<QPointF> points;
    QPointF first(x+cos(angle_)*ARROWWIDTH,
                  y+sin(angle_)*ARROWWIDTH);
    points.append(first);
    double tempAngle = angle_+40.0*PI/180.0;
    points.append(QPointF(x-cos(tempAngle)*ARROWWIDTH,
                          y-sin(tempAngle)*ARROWWIDTH));
    tempAngle -= 80.0*PI/180.0;
    points.append(QPointF(x-cos(tempAngle)*ARROWWIDTH,
                          y-sin(tempAngle)*ARROWWIDTH));
    points.append(first);
    QPolygonF triangle(points);
    //calculate the point at the back of the triangle
    double triangleX = (points.at(1).x()+points.at(2).x())/2.0;
    double triangleY = (points.at(1).y()+points.at(2).y())/2.0;

    QGraphicsLineItem* traceL = new QGraphicsLineItem
            (initX_, initY_, x, y);
    QPen linePen(Qt::GlobalColor::gray);
    linePen.setWidth(TRACEWIDTH);
    traceL->setPen(linePen);
    traceL->setOpacity(0.25);
    traces_.append(traceL);
    scene()->addItem(traceL);

    if (!traceShown_)
    {
        traceL->setVisible(false);
    }

    if (curPoint_ == NULL)  //first update
    {
        curPoint_ = scene()->addPolygon(triangle);
        //color of the roombaTriangle is blue
        QBrush triangleBrush(Qt::GlobalColor::blue);
        curPoint_->setBrush(triangleBrush);
        QPen curSpeedPen(Qt::GlobalColor::blue);
        curSpeedPen.setWidth(TRACEWIDTH/4.0);
        curSpeed_ = scene()->addLine(triangleX, triangleY,
                                       triangleX-cos(angle_)*ARROWWIDTH*2.0*speed,
                                       triangleY-sin(angle_)*ARROWWIDTH*2.0*speed,
                                       curSpeedPen);
    }
    else
    {
        curPoint_->setPolygon(triangle);
        curSpeed_->setLine(triangleX, triangleY,
                           triangleX-cos(angle_)*ARROWWIDTH*2.0*speed,
                           triangleY-sin(angle_)*ARROWWIDTH*2.0*speed);
    }

    curPoint_->setZValue(1);
    curSpeed_->setZValue(1);
    initX_ = x;
    initY_ = y;
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

void mapQGraphicsView::resetAngle()
{
    angle_ = 0.0;
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

QPointF mapQGraphicsView::getRoombasLocation()
{
    //x = curPoint_->pos().x();
    //y = curPoint_->pos().y();
    QPointF point(initX_, initY_);
    return point;

}

double mapQGraphicsView::getCurrentAngle()
{
    return angle_;
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
//    delete curPoint_;
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
