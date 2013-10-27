#include <QPointF>
#include <QReadLocker>
#include "poiQGraphicsEllipseItem.h"
#include <set>
#include <iostream>
#include "mainwindow.h"
#include "mapQGraphicsView.h"
#include <cmath>
#include <QDebug>

mapQGraphicsView::mapQGraphicsView(QWidget* parent) :
    QGraphicsView(parent), startPoint_(NULL), curPoint_(NULL),
    curSpeed_(NULL), initX_(0), initY_(0), angle_(0.0), traceShown_(true)
{
    mapScene_ = new QGraphicsScene(childrenRect(), this);
    setScene(mapScene_);
    setRenderHints(QPainter::Antialiasing);
}

void mapQGraphicsView::removePoi(poiQGraphicsEllipseItem* poi)
{
    mapScene_->removeItem(poi);
    pois_.erase(poi);
    delete poi;
}

void mapQGraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QPointF point = mapToScene(event->pos());
    //QPointF point = mapTo(this, event->pos());
    //QPointF point = event->pos();

    if(startPoint_ == NULL)
    {
        initX_ = point.x();
        initY_ = point.y();
        QGraphicsEllipseItem* startPoint = new QGraphicsEllipseItem
                (point.x()-TRACEWIDTH, point.y()-TRACEWIDTH, TRACEWIDTH*2,
                 TRACEWIDTH*2);
        QBrush brush(Qt::GlobalColor::green);
        startPoint->setBrush(brush);
        mapScene_->addItem(startPoint);
        startPoint_ = startPoint;
    }
    else
    {
        poiQGraphicsEllipseItem* poi = new poiQGraphicsEllipseItem
                (point.x()-POIWIDTH/2, point.y()-POIWIDTH/2, POIWIDTH,
                 POIWIDTH, this);
        mapScene_->addItem(poi);
        pois_.insert(poi);
    }
}

void mapQGraphicsView::clearRedPois()
{
    for (std::set<poiQGraphicsEllipseItem*>::iterator i = pois_.begin();
        i != pois_.end(); ++i)
    {
        if ((*i)->pen().color() == Qt::GlobalColor::red)
        {
            pois_.erase(i);
            mapScene_->removeItem(*i);
            delete *i;
        }
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
            //mapScene_->removeItem(*i);
        }
    }
    else
    {
        traceShown_ = true;

        for (QVector<QGraphicsLineItem*>::iterator i = traces_.begin();
            i != traces_.end(); ++i)
        {
            (*i)->setVisible(true);
            //mapScene_->addItem(*i);
        }
    }
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
    //magic scaling due to currently nonscaled coordinates
    double dist = -(static_cast<double>(distance)/MAGICSCALE);
    //special radiuses mean no adaptation needed
    if (radius != 32768 && radius != 32767 && radius != -1 && radius != 1)
    {
        //corrected distance
        dist = -2.0*(static_cast<double>(radius)/MAGICSCALE)*
                sin(static_cast<double>(distance)/radius/2);
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

    QPointF traceP(initX_, initY_);

    if (curPoint_ == NULL)  //first update
    {
        curPoint_ = mapScene_->addPolygon(triangle);
        //color of the roombaTriangle is blue
        QBrush triangleBrush(Qt::GlobalColor::blue);
        curPoint_->setBrush(triangleBrush);
        QPen curSpeedPen(Qt::GlobalColor::blue);
        curSpeedPen.setWidth(TRACEWIDTH/2.0);
        curSpeed_ = mapScene_->addLine(triangleX, triangleY,
                                       triangleX-cos(angle_)*ARROWWIDTH*4.0*speed,
                                       triangleY-sin(angle_)*ARROWWIDTH*4.0*speed,
                                       curSpeedPen);
    }
    else
    {
        QGraphicsLineItem* traceL = new QGraphicsLineItem
                (initX_, initY_, x, y);
        QPen linePen(Qt::GlobalColor::red);
        linePen.setWidth(TRACEWIDTH);
        traceL->setPen(linePen);
        traces_.append(traceL);
        if (traceShown_)
        {
            mapScene_->addItem(traceL);
        }

        curPoint_->setPolygon(triangle);
        curSpeed_->setLine(triangleX, triangleY,
                           triangleX-cos(angle_)*ARROWWIDTH*4.0*speed,
                           triangleY-sin(angle_)*ARROWWIDTH*4.0*speed);
    }

    curPoint_->setZValue(1);
    curSpeed_->setZValue(1);
    initX_ = x;
    initY_ = y;
}

mapQGraphicsView::~mapQGraphicsView()
{ 
    delete curPoint_;

    for (QVector<QGraphicsLineItem*>::iterator i = traces_.begin();
        i != traces_.end(); ++i)
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
