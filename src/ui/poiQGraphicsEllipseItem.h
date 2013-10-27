#ifndef POIQGRAPHICSELLIPSEITEM_H
#define POIQGRAPHICSELLIPSEITEM_H

#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include "mapQGraphicsView.h"

class mapQGraphicsView;

class Q_WIDGETS_EXPORT poiQGraphicsEllipseItem:
        public QGraphicsEllipseItem
{
//Q_OBJECT //causes errors, perhaps not needed with Q_WIDGETS_EXPORT?
public:
    //explicit poiQGraphicsEllipseItem(QGraphicsItem *parent = 0);
    explicit poiQGraphicsEllipseItem
    (qreal x, qreal y, qreal w, qreal h, mapQGraphicsView* map, QGraphicsItem* parent = 0);

    void mousePressEvent(QGraphicsSceneMouseEvent* event);

    virtual ~poiQGraphicsEllipseItem();
signals:

public slots:

private:
    mapQGraphicsView* map_;
};

#endif // POIQGRAPHICSELLIPSEITEM_H

