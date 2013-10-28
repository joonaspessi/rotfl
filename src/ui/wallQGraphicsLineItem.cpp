#include "wallQGraphicsLineItem.h"
#include "mapQGraphicsView.h"

wallQGraphicsLineItem::wallQGraphicsLineItem
    (qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem *parent):
        QGraphicsLineItem(x1, y1, x2, y2, parent)
{
    QPen pen(Qt::GlobalColor::black);
    pen.setWidth(3);
    setPen(pen);
    setZValue(1);
}

void wallQGraphicsLineItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        return;
    }

    if (pen().color() == Qt::GlobalColor::black)
    {
        QPen pen(Qt::GlobalColor::red);
        pen.setWidth(3);
        setPen(pen);
    }
    else
    {
        QPen pen(Qt::GlobalColor::black);
        pen.setWidth(3);
        setPen(pen);
    }
}

wallQGraphicsLineItem::~wallQGraphicsLineItem()
{
}
