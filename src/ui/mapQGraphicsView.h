#ifndef MAPQGRAPHICSVIEW_H
#define MAPQGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include "poiQGraphicsEllipseItem.h"
#include <QMouseEvent>
#include <set>

const double POIWIDTH = 10.0;
const double TRACEWIDTH = 10.0;
const double PI = 3.14159265;
const double MAGICSCALE = 5.0;
class poiQGraphicsEllipseItem;

class mapQGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit mapQGraphicsView(QWidget* parent = 0);
    void removePoi(poiQGraphicsEllipseItem* poi);
    //updates roomba's location and heading. Keeps a trace
    //Remember following special radiuses
    //Straight = 32768 or 32767
    //Turn in place clockwise = 65535
    //Turn in place counter-clockwise = 1
    void updateLoc(int distance, int angle, int radius);

    void clearAllPois();
    void clearAllTraces();
    virtual ~mapQGraphicsView();

public slots:
    void mousePressEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);

private:
    QGraphicsScene* mapScene_;
    std::set<poiQGraphicsEllipseItem*> pois_;
    std::set<QGraphicsEllipseItem*> traces_;
    QGraphicsEllipseItem* startPoint_;
    QGraphicsPolygonItem* curPoint_;
    double initX_;
    double initY_;
    double angle_;
};

#endif // MAPQGRAPHICSVIEW_H
