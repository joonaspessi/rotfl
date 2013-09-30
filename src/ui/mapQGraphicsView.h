#ifndef MAPQGRAPHICSVIEW_H
#define MAPQGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include "poiQGraphicsEllipseItem.h"
#include <QMouseEvent>
#include <set>

class poiQGraphicsEllipseItem;

class mapQGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit mapQGraphicsView(QWidget* parent = 0);
    virtual ~mapQGraphicsView();

    void removePoi(poiQGraphicsEllipseItem* poi);
signals:

public slots:
    void mousePressEvent(QMouseEvent* event);
private:
    QGraphicsScene * mapScene_;
    std::set<poiQGraphicsEllipseItem*> pois_;
};

#endif // MAPQGRAPHICSVIEW_H
