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
    mapWidth_(600), traceShown_(true)
{
    mapScene_ = new QGraphicsScene(childrenRect(), this);
    setScene(mapScene_);
    setRenderHints(QPainter::Antialiasing);
    //the view's rectangle's size 400 should instead be taken with function
    //currently the functions give an unwanted value for some reason
    scale(400.0/mapWidth_, 400.0/mapWidth_);
    qDebug() << rect().width();
}

void mapQGraphicsView::removePoi(poiQGraphicsEllipseItem* poi)
{
    mapScene_->removeItem(poi);
    pois_.erase(poi);
    delete poi;
}

void mapQGraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QPointF p = mapToScene(event->pos());

    if (event->button() == Qt::LeftButton)
    {
        if(startPoint_ == NULL)
        {
            initX_ = p.x();
            initY_ = p.y();
            startPoint_ = new poiQGraphicsEllipseItem
                    (p.x()-POIWIDTH*2/3, p.y()-POIWIDTH*2/3, POIWIDTH*4/3, POIWIDTH*4/3);
            QBrush brush(Qt::GlobalColor::green);
            startPoint_->setBrush(brush);
            mapScene_->addItem(startPoint_);
        }
        else
        {
            poiQGraphicsEllipseItem* poi = new poiQGraphicsEllipseItem
                    (p.x()-POIWIDTH/2, p.y()-POIWIDTH/2, POIWIDTH, POIWIDTH);
            mapScene_->addItem(poi);
            pois_.insert(poi);
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if (wallStartPoint_ == NULL)
        {
            wallStartPoint_ = new QGraphicsRectItem(0, 0, 2, 2);
            wallStartPoint_->setPos(p);
            mapScene_->addItem(wallStartPoint_);
        }
        else
        {
            wallQGraphicsLineItem* wall = new wallQGraphicsLineItem
                    (wallStartPoint_->x(), wallStartPoint_->y(), p.x(), p.y());
            walls_.insert(wall);
            mapScene_->addItem(wall);
            mapScene_->removeItem(wallStartPoint_);
            delete wallStartPoint_;
            wallStartPoint_ = NULL;
        }
    }
}

void mapQGraphicsView::removeRedObjects()
{
    for (std::set<poiQGraphicsEllipseItem*>::iterator i = pois_.begin();
        i != pois_.end(); ++i)
    {
        if ((*i)->pen().color() == Qt::GlobalColor::red)
        {
            mapScene_->removeItem(*i);
            delete *i;
            pois_.erase(i);
        }
    }
    for (std::set<wallQGraphicsLineItem*>::iterator i = walls_.begin();
        i != walls_.end(); ++i)
    {
        if ((*i)->pen().color() == Qt::GlobalColor::red)
        {
            mapScene_->removeItem(*i);
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
        mapScene_->removeItem(curPoint_);
        delete curPoint_;
        curPoint_ = NULL;
        mapScene_->removeItem(curSpeed_);
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
        mapScene_->removeItem(*i);
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
    double dist = -static_cast<double>(distance)/10.0;
    //special radiuses mean no adaptation needed
    if (radius != 32768 && radius != 32767 && radius != -1 && radius != 1)
    {
        //corrected distance (and change to cm)
        dist = -2.0*(static_cast<double>(radius))*
                sin(static_cast<double>(distance)/radius/2)/10.0;
        //corrected angle in radians for distance calculation
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
    traces_.append(traceL);
    mapScene_->addItem(traceL);

    if (!traceShown_)
    {
        traceL->setVisible(false);
    }

    if (curPoint_ == NULL)  //first update
    {
        curPoint_ = mapScene_->addPolygon(triangle);
        //color of the roombaTriangle is blue
        QBrush triangleBrush(Qt::GlobalColor::blue);
        curPoint_->setBrush(triangleBrush);
        QPen curSpeedPen(Qt::GlobalColor::blue);
        curSpeedPen.setWidth(TRACEWIDTH/4.0);
        curSpeed_ = mapScene_->addLine(triangleX, triangleY,
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
}

//gives map's width in mm
int mapQGraphicsView::giveMapWidth()
{
    return mapWidth_;
}

//give new width in mm.
void mapQGraphicsView::changeMapWidth(int width)
{
    mapWidth_ = width;
    //the view's rectangle's size 400.0 should instead be taken with function.
    //currently the functions give an unwanted value for some reason
    resetTransform();
    scale(400.0/mapWidth_, 400.0/mapWidth_);
}

void mapQGraphicsView::resetAngle()
{
    angle_ = 0.0;
}

mapQGraphicsView::~mapQGraphicsView()
{ 
    delete curPoint_;
    removeTraces();

    for (std::set<wallQGraphicsLineItem*>::iterator i = walls_.begin();
        i != walls_.end(); ++i)
    {
        mapScene_->removeItem(*i);
        delete *i;
    }

    for (std::set<poiQGraphicsEllipseItem*>::iterator i = pois_.begin();
        i != pois_.end(); ++i)
    {
        mapScene_->removeItem(*i);
        delete *i;
    }
}
