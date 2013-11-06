#ifndef POIQGRAPHICSELLIPSEITEM_H
#define POIQGRAPHICSELLIPSEITEM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

class Q_WIDGETS_EXPORT poiQGraphicsEllipseItem:
        public QGraphicsEllipseItem
{
public:
    explicit poiQGraphicsEllipseItem
    (qreal x, qreal y, qreal w, qreal h, QGraphicsItem* parent = 0);

    virtual ~poiQGraphicsEllipseItem();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:

};

#endif // POIQGRAPHICSELLIPSEITEM_H

