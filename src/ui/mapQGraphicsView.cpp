#include <QPointF>
#include <QReadLocker>
#include "poiQGraphicsEllipseItem.h"
#include <set>
#include <iostream>
#include "mainwindow.h"
#include "mapQGraphicsView.h"

mapQGraphicsView::mapQGraphicsView(QWidget* parent) :
                    QGraphicsView(parent)
{
    mapScene_ = new QGraphicsScene();
    setScene(mapScene_);
    setSceneRect(0, 0, 300, 300);
}

void mapQGraphicsView::removePoi(poiQGraphicsEllipseItem* poi)
{
    mapScene_->removeItem(poi);
    pois_.erase(poi);
    delete poi;
}

void mapQGraphicsView::mousePressEvent(QMouseEvent* event)
{
    event->ignore();
}

void mapQGraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
    double width = 10.0;
    QPointF point = mapToScene(event->pos());
    poiQGraphicsEllipseItem* poi = new poiQGraphicsEllipseItem
            (point.x()-width/2, point.y()-width/2, width, width, this);
    mapScene_->addItem(poi);
    pois_.insert(poi);
}

void mapQGraphicsView::clearAllPois()
{
    for (std::set<poiQGraphicsEllipseItem*>::iterator i = pois_.begin();
        i != pois_.end(); ++i)
    {
        mapScene_->removeItem(*i);
        delete *i;
    }
    pois_.clear();
}

mapQGraphicsView::~mapQGraphicsView()
{
    clearAllPois();
}
