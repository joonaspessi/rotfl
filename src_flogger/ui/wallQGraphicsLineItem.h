#ifndef WALLQGRAPHICSLINEITEM_H
#define WALLQGRAPHICSLINEITEM_H

#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>

class Q_WIDGETS_EXPORT wallQGraphicsLineItem:
        public QGraphicsLineItem
{
public:
    explicit wallQGraphicsLineItem
    (qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem *parent = 0);

    virtual ~wallQGraphicsLineItem();
signals:

public slots:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

private:
};

#endif // WALLQGRAPHICSLINEITEM_H
