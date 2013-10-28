#include "poiQGraphicsEllipseItem.h"
#include "mapQGraphicsView.h"

poiQGraphicsEllipseItem::poiQGraphicsEllipseItem
    (qreal x, qreal y, qreal w, qreal h, QGraphicsItem* parent):
        QGraphicsEllipseItem(x, y, w, h, parent)
{    
    QPen pen(Qt::GlobalColor::darkCyan);
    pen.setWidth(2);
    setPen(pen);
    setZValue(1);
}

void poiQGraphicsEllipseItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        return;
    }

    if (pen().color() == Qt::GlobalColor::darkCyan)
    {
        QPen pen(Qt::GlobalColor::red);
        pen.setWidth(2);
        setPen(pen);
    }
    else
    {
        QPen pen(Qt::GlobalColor::darkCyan);
        pen.setWidth(2);
        setPen(pen);
    }
}

poiQGraphicsEllipseItem::~poiQGraphicsEllipseItem()
{
}

