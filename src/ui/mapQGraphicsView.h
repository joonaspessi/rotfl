#ifndef MAPQGRAPHICSVIEW_H
#define MAPQGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include "poiQGraphicsEllipseItem.h"
#include "wallQGraphicsLineItem.h"
#include <QMouseEvent>

#include <set>

const double POIWIDTH = 10.0;
const double TRACEWIDTH = 34.0;
const double ARROWWIDTH = 27.0;
const double PI = 3.14159265;
const double ANGLECORRECTION = 3.05;
const double DISTANCECORRECTION = 6.1;

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
    //Turn in place clockwise = -1
    //Turn in place counter-clockwise = 1
    void updateLoc(int distance, int angle, int radius, int velocity);
    void removeRedObjects();
    void ifShowTraces();
    void removeTraces();
    //gives map's width in cm
    int giveMapWidth();
    //give new width in cm
    void setMapWidth(int width);
    void resetAngle();

    QPointF getNextPoi();
    QPointF getRoombasLocation();
    double getCurrentAngle();
    void checkPoiCollision();

    virtual ~mapQGraphicsView();

public slots:
    void mouseDoubleClickEvent(QMouseEvent* event);

private:
    QGraphicsRectItem* wallStartPoint_;
    std::set<wallQGraphicsLineItem*> walls_;
    std::set<poiQGraphicsEllipseItem*> pois_;
    QVector<QGraphicsLineItem*> traces_;
    poiQGraphicsEllipseItem* startPoint_;
    QGraphicsPolygonItem* curPoint_;
    QGraphicsLineItem* curSpeed_;
    double initX_;
    double initY_;
    double angle_;
    int mapWidth_;
    bool traceShown_;
};

#endif // MAPQGRAPHICSVIEW_H
