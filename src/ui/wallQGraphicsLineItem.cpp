#include "wallQGraphicsLineItem.h"
#include "mapQGraphicsView.h"
#include <QStyleOption>

WallQGraphicsLineItem::WallQGraphicsLineItem
(qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem *parent):
    QGraphicsLineItem(x1, y1, x2, y2, parent)
{
    setZValue(1);
}

void WallQGraphicsLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QStyleOptionGraphicsItem myoption = (*option);
    myoption.state &= !QStyle::State_Selected;
    QPen pen(Qt::GlobalColor::black);
    pen.setWidth(3);
    if (isSelected()) {
        pen.setColor(Qt::GlobalColor::red);
    }
    this->setPen(pen);
    QGraphicsLineItem::paint(painter, &myoption, widget);
}

WallQGraphicsLineItem::~WallQGraphicsLineItem()
{
}
