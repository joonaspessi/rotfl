#include "poiQGraphicsEllipseItem.h"
#include "mapQGraphicsView.h"
#include <QDebug>
#include <QStyleOption>

poiQGraphicsEllipseItem::poiQGraphicsEllipseItem
(qreal x, qreal y, qreal w, qreal h, QGraphicsItem* parent):
    QGraphicsEllipseItem(x, y, w, h, parent)
{    
    setZValue(1);
}

void poiQGraphicsEllipseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QStyleOptionGraphicsItem myoption = (*option);
    myoption.state &= !QStyle::State_Selected;
    QPen pen(Qt::GlobalColor::darkCyan);
    pen.setWidth(2);
    if (isSelected()) {
        pen.setColor(Qt::GlobalColor::red);
    }
    this->setPen(pen);
    QGraphicsEllipseItem::paint(painter, &myoption, widget);
}

poiQGraphicsEllipseItem::~poiQGraphicsEllipseItem()
{
}

