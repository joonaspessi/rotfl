#include "poiQGraphicsEllipseItem.h"
#include "mapQGraphicsView.h"
#include <QDebug>
#include <QStyleOption>
#include <QMenu>
#include <uiUtils.h>
#include <fleetManager.h>

PoiQGraphicsEllipseItem::PoiQGraphicsEllipseItem
(FleetManager* fleetManager, qreal x, qreal y, qreal w, qreal h, QGraphicsItem* parent):
    QGraphicsEllipseItem(x, y, w, h, parent), fleetManager_(fleetManager),
    gettingCollected_(false)
{
    setZValue(1);
}

void PoiQGraphicsEllipseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QStyleOptionGraphicsItem myoption = (*option);
    myoption.state &= !QStyle::State_Selected;
    QPen pen(Qt::GlobalColor::darkCyan);
    pen.setWidth(2);
    if (isSelected()) {
        pen.setColor(Qt::GlobalColor::red);
    }
    this->setPen(pen);
    QGraphicsEllipseItem::paint(painter, &myoption, widget);
}

int PoiQGraphicsEllipseItem::type() const
{
    return Util::POITYPE;
}

void PoiQGraphicsEllipseItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *removeAction = menu.addAction("Remove");
    QAction *selectedAction = menu.exec(event->screenPos());

    if (selectedAction == removeAction)
    {
        fleetManager_->removePoi(this);
    }
}

bool PoiQGraphicsEllipseItem::getGettingCollected()
{
    return gettingCollected_;
}

void PoiQGraphicsEllipseItem::setGettingCollected(bool gettingCollected)
{
    gettingCollected_ = gettingCollected;
}

PoiQGraphicsEllipseItem::~PoiQGraphicsEllipseItem()
{
}

