#ifndef ATCQGRAPHICSRECTITEM_H
#define ATCQGRAPHICSRECTITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>

class FleetManager;
class Q_WIDGETS_EXPORT AtcQGraphicsRectItem:
        public QGraphicsRectItem
{
public:
    explicit AtcQGraphicsRectItem
    (FleetManager *fleetManager, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = 0);
    bool isGettingCleaned();
    void setGettingCleaned();

    virtual ~AtcQGraphicsRectItem();
    //reason for this reimplementation is to have self-defined color (red) on select
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
  int type() const;
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
    bool gettingCleaned_;
    FleetManager* fleetManager_;
};

#endif // ATCQGRAPHICSRECTITEM_H
