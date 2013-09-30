#ifndef POIQGRAPHICSELLIPSEITEM_H
#define POIQGRAPHICSELLIPSEITEM_H

#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include "mapQGraphicsView.h"

class mapQGraphicsView;

class Q_WIDGETS_EXPORT poiQGraphicsEllipseItem : public QGraphicsEllipseItem
{
public:
    //explicit poiQGraphicsEllipseItem(QGraphicsItem *parent = 0);
    explicit poiQGraphicsEllipseItem
    (qreal x, qreal y, qreal w, qreal h, mapQGraphicsView* map, QGraphicsItem* parent = 0);

    virtual ~poiQGraphicsEllipseItem();
signals:

public slots:
    void mouseReleaseEvent(QMouseEvent* event);

private:
    mapQGraphicsView* map_;
};

#endif // POIQGRAPHICSELLIPSEITEM_H

