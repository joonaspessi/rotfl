#include "wallQGraphicsLineItem.h"
#include "mapQGraphicsView.h"
#include <QStyleOption>
#include <QMenu>
#include <uiUtils.h>
#include <fleetManager.h>

WallQGraphicsLineItem::WallQGraphicsLineItem
(FleetManager* fleetManager, qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem *parent):
    QGraphicsLineItem(x1, y1, x2, y2, parent), fleetManager_(fleetManager)
{
    setZValue(1);
    setPen(QPen(Qt::GlobalColor::black, 3));
}

int WallQGraphicsLineItem::type() const
{
    return Util::WALLTYPE;
}

void WallQGraphicsLineItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *removeAction = menu.addAction("Remove");
    QAction *selectedAction = menu.exec(event->screenPos());

    if (selectedAction == removeAction)
    {
        fleetManager_->removeWall(this);
    }
}

WallQGraphicsLineItem::~WallQGraphicsLineItem()
{
}
