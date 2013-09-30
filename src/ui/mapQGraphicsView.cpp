#include "mapQGraphicsView.h"
#include <QPointF>
#include <QReadLocker>
#include <QRectF>
#include "poiQGraphicsEllipseItem.h"
#include <set>

mapQGraphicsView::mapQGraphicsView(QWidget* parent) :
                    QGraphicsView(parent)
{
    mapScene_ = new QGraphicsScene();
    this->setSceneRect(0, 0, 500, 500);
    this->setScene(mapScene_);
}

void mapQGraphicsView::removePoi(poiQGraphicsEllipseItem* poi)
{
    pois_.erase(poi);
}

void mapQGraphicsView::mousePressEvent(QMouseEvent * event)
{
    double width = 10.0;
    QPointF point = mapToScene(event->pos());
    poiQGraphicsEllipseItem* poi = new poiQGraphicsEllipseItem
            (point.x()-width/2, point.y()-width/2, width, width, this);
    mapScene_->addItem(poi);
    pois_.insert(poi);
}

mapQGraphicsView::~mapQGraphicsView()
{
    for (std::set<poiQGraphicsEllipseItem*>::iterator i = pois_.begin();
        i != pois_.end(); ++i)
    {
        delete *i;
    }
}
