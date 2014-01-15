#ifndef ATCQGRAPHICSRECTITEM_H
#define ATCQGRAPHICSRECTITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>

class Q_WIDGETS_EXPORT AtcQGraphicsRectItem:
        public QGraphicsRectItem
{
public:
    explicit AtcQGraphicsRectItem
    (qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = 0);
    bool isGettingCleaned();
    void setGettingCleaned();

    virtual ~AtcQGraphicsRectItem();
    //reason for this reimplementation is to have self-defined color (red) on select
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    bool gettingCleaned_;
};

#endif // ATCQGRAPHICSRECTITEM_H
