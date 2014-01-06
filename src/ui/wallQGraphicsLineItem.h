#ifndef WALLQGRAPHICSLINEITEM_H
#define WALLQGRAPHICSLINEITEM_H

#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>

class FleetManager;

class Q_WIDGETS_EXPORT WallQGraphicsLineItem:
        public QGraphicsLineItem
{
public:
    explicit WallQGraphicsLineItem
    (FleetManager *fleetManager, qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem *parent = 0);

    virtual ~WallQGraphicsLineItem();
    //reason for this reimplementation is to have self-defined color (red) on select
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    int type() const;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
    FleetManager* fleetManager_;
};

#endif // WALLQGRAPHICSLINEITEM_H
