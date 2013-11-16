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
signals:

public slots:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

private:

};

#endif // POIQGRAPHICSELLIPSEITEM_H

