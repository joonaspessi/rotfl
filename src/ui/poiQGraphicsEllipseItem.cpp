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
}

//void poiQGraphicsEllipseItem::mousePressEvent(QMouseEvent* event)
//{
//    map_->removePoi(this);
//}

poiQGraphicsEllipseItem::~poiQGraphicsEllipseItem()
{
}

