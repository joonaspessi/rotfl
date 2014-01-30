#ifndef ATCQGRAPHICSRECTITEM_H
#define ATCQGRAPHICSRECTITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>

class FleetManager;
class AtcQGraphicsRectItem:
        public QGraphicsRectItem
{
public:
    explicit AtcQGraphicsRectItem
    (FleetManager *fleetManager, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = 0);
    bool isGettingCleaned();
    void setGettingCleaned();

    virtual ~AtcQGraphicsRectItem();
    int type() const;
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
    bool gettingCleaned_;
    FleetManager* fleetManager_;
};

#endif // ATCQGRAPHICSRECTITEM_H
