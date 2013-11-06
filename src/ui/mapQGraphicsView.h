#ifndef MAPQGRAPHICSVIEW_H
#define MAPQGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include "poiQGraphicsEllipseItem.h"
#include "wallQGraphicsLineItem.h"
#include <QMouseEvent>
#include "croi/iRoomba.h"
#include <set>

//SOME OF THESE CONSTANTS ARE TO BE MOVED
//AND SOME CHANGED TO VARIABLES
const double POIWIDTH = 10.0;
const double TRACEWIDTH = 34.0;
const double ARROWWIDTH = 27.0;
const double PI = 3.14159265;
const double ANGLECORRECTION = 3.05;
const double DISTANCECORRECTION = 6.1;

class poiQGraphicsEllipseItem;

//NOTE: handles all additions and deletions from scene
class mapQGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit mapQGraphicsView(QWidget* parent = NULL);
    void removePoi(poiQGraphicsEllipseItem* poi);
    //updates roomba's location and heading in map. Keeps a trace
    void updateLoc(QVector<Croi::IRoomba *>* roombas);
    void removeRedObjects();
    void ifShowTraces(QVector<Croi::IRoomba*>* roombas);
    void removeTraces(QVector<Croi::IRoomba*>* roombas);
    //gives map's width in cm
    int giveMapWidth();
    //give new width in cm
    void setMapWidth(int width);

    QPointF getNextPoi();
    void checkPoiCollision();

    virtual ~mapQGraphicsView();

public slots:
    //void mouseDoubleClickEvent(QMouseEvent* event);

private:
    QGraphicsRectItem* wallStartPoint_;
    std::set<wallQGraphicsLineItem*> walls_;
    std::set<poiQGraphicsEllipseItem*> pois_;
    Croi::IRoomba* selectedRoomba_;
    int mapWidth_;
    bool traceShown_;
};

#endif // MAPQGRAPHICSVIEW_H
