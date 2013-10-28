#include "poiQGraphicsEllipseItem.h"
#include "mapQGraphicsView.h"

poiQGraphicsEllipseItem::poiQGraphicsEllipseItem
    (qreal x, qreal y, qreal w, qreal h, QGraphicsItem* parent):
        QGraphicsEllipseItem(x, y, w, h, parent)
{    
    QPen pen(Qt::GlobalColor::black);
    setPen(pen);
    setZValue(1);
}

void poiQGraphicsEllipseItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        return;
    }

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
}

poiQGraphicsEllipseItem::~poiQGraphicsEllipseItem()
{
}

