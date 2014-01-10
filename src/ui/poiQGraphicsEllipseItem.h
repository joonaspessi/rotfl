#ifndef POIQGRAPHICSELLIPSEITEM_H
#define POIQGRAPHICSELLIPSEITEM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

class FleetManager;

class Q_WIDGETS_EXPORT PoiQGraphicsEllipseItem:
        public QGraphicsEllipseItem
{
public:
    explicit PoiQGraphicsEllipseItem
    (FleetManager *fleetManager, qreal x, qreal y, qreal w, qreal h, QGraphicsItem* parent = 0);

    virtual ~PoiQGraphicsEllipseItem();
    //reason for this reimplementation is to have self-defined color (red) on select
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    int type() const;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    bool getGettingCollected();
    void setGettingCollected(bool gettingCollected);

private:
    FleetManager* fleetManager_;
    bool gettingCollected_;
};

#endif // POIQGRAPHICSELLIPSEITEM_H

