#ifndef POIQGRAPHICSELLIPSEITEM_H
#define POIQGRAPHICSELLIPSEITEM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

class Q_WIDGETS_EXPORT PoiQGraphicsEllipseItem:
        public QGraphicsEllipseItem
{
public:
    explicit PoiQGraphicsEllipseItem
    (qreal x, qreal y, qreal w, qreal h, QGraphicsItem* parent = 0);

    virtual ~PoiQGraphicsEllipseItem();
    //reason for this reimplementation is to have self-defined color (red) on select
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:

};

#endif // POIQGRAPHICSELLIPSEITEM_H

