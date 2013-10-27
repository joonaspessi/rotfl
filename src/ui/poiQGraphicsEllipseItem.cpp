#include <QGraphicsEllipseItem>

#include "poiQGraphicsEllipseItem.h"
#include "mapQGraphicsView.h"
#include <iostream>

poiQGraphicsEllipseItem::poiQGraphicsEllipseItem
    (qreal x, qreal y, qreal w, qreal h, mapQGraphicsView* map,
     QGraphicsItem* parent) :
        QGraphicsEllipseItem(x, y, w, h, parent)
{    
    map_ = map;
    QPen pen(Qt::GlobalColor::black);
    setPen(pen);
}

void poiQGraphicsEllipseItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (pen().color() == Qt::GlobalColor::black)
    {
        QPen pen(Qt::GlobalColor::red);
        setPen(pen);
    }
    else
    {
        QPen pen(Qt::GlobalColor::black);
        setPen(pen);
    }
    //map_->removePoi(this);
}

poiQGraphicsEllipseItem::~poiQGraphicsEllipseItem()
{
}

