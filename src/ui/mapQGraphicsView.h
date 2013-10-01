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
    void removePoi(poiQGraphicsEllipseItem* poi);
    virtual ~mapQGraphicsView();

public slots:
    void mousePressEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void clearAllPois();

private:
    QGraphicsScene* mapScene_;
    std::set<poiQGraphicsEllipseItem*> pois_;
};

#endif // MAPQGRAPHICSVIEW_H
