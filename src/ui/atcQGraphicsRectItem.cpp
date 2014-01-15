#include "atcQGraphicsRectItem.h"
#include "mapQGraphicsView.h"
#include <QStyleOption>

AtcQGraphicsRectItem::AtcQGraphicsRectItem
(qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent):
    QGraphicsRectItem(x, y, w, h, parent), gettingCleaned_(false)
{
    setZValue(1);
}


void AtcQGraphicsRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QStyleOptionGraphicsItem myoption = (*option);
    myoption.state &= !QStyle::State_Selected;
    QPen pen(Qt::GlobalColor::black, 3, Qt::DotLine);

    /*
    m_outlineItem = new QGraphicsRectItem(m_svgItem->boundingRect());
        QPen outline(Qt::black, 2, Qt::DashLine);
        outline.setCosmetic(true);
        m_outlineItem->setPen(outline);
        m_outlineItem->setBrush(Qt::NoBrush);
        m_outlineItem->setVisible(drawOutline);
        m_outlineItem->setZValue(1);
*/

  //  pen.setWidth(2);
    if (isSelected()) {
        pen.setColor(Qt::GlobalColor::red);
    }
    this->setPen(pen);
    QGraphicsRectItem::paint(painter, &myoption, widget);
}

bool AtcQGraphicsRectItem::isGettingCleaned()
{
    return gettingCleaned_;
}

void AtcQGraphicsRectItem::setGettingCleaned()
{
    gettingCleaned_ = true;
}

AtcQGraphicsRectItem::~AtcQGraphicsRectItem()
{
}
