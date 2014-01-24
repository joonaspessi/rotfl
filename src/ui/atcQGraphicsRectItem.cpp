#include "atcQGraphicsRectItem.h"
#include "mapQGraphicsView.h"
#include "fleetManager.h"
#include "uiUtils.h"
#include <QStyleOption>
#include <QMenu>

AtcQGraphicsRectItem::AtcQGraphicsRectItem
(FleetManager* fleetManager, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent):
    QGraphicsRectItem(x, y, w, h, parent), gettingCleaned_(false), fleetManager_(fleetManager)
{
    setZValue(1);
    setPen(QPen(Qt::GlobalColor::black, 3, Qt::DotLine));
}

bool AtcQGraphicsRectItem::isGettingCleaned()
{
    return gettingCleaned_;
}

void AtcQGraphicsRectItem::setGettingCleaned()
{
    gettingCleaned_ = true;
}
int AtcQGraphicsRectItem::type() const
{
    return Util::ATCTYPE;
}

void AtcQGraphicsRectItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *removeAction = menu.addAction("Remove");
    QAction *selectedAction = menu.exec(event->screenPos());

    if (selectedAction == removeAction)
    {
        fleetManager_->removeAtc(this);
    }
}

AtcQGraphicsRectItem::~AtcQGraphicsRectItem()
{
}
