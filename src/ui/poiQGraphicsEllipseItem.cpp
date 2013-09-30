#include <QGraphicsEllipseItem>

#include "poiQGraphicsEllipseItem.h"
#include "mapQGraphicsView.h"

poiQGraphicsEllipseItem::poiQGraphicsEllipseItem
    (qreal x, qreal y, qreal w, qreal h, mapQGraphicsView* map, QGraphicsItem* parent) :
        QGraphicsEllipseItem(x, y, w, h, parent)
{    
    map_ = map;
}

void poiQGraphicsEllipseItem::mouseReleaseEvent(QMouseEvent* event)
{
    map_->removePoi(this);
    delete this;
}

poiQGraphicsEllipseItem::~poiQGraphicsEllipseItem()
{
}

