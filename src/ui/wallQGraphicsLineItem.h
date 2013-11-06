#ifndef WALLQGRAPHICSLINEITEM_H
#define WALLQGRAPHICSLINEITEM_H

#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>

class Q_WIDGETS_EXPORT wallQGraphicsLineItem:
        public QGraphicsLineItem
{
public:
    explicit wallQGraphicsLineItem
    (qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem *parent = 0);

    virtual ~wallQGraphicsLineItem();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
};

#endif // WALLQGRAPHICSLINEITEM_H
