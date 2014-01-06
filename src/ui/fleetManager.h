#ifndef FLEETMANAGER_H
#define FLEETMANAGER_H

//#include "mainwindow.h"
#include <QObject>
#include <croi/iRoomba.h>
//#include "mapQGraphicsView.h"
#include "wallQGraphicsLineItem.h"
#include "poiQGraphicsEllipseItem.h"
#include <set>
#include <string>
#include <QTimer>
#include <limits>
#include "uiUtils.h"

class MainWindow;
class MapQGraphicsView;

class FleetManager : public QObject
{
    Q_OBJECT
public:
    explicit FleetManager(MainWindow* mainWindow, QObject *parent = NULL);

    //makes new roomba, adds it to class variables and
    //also returns a pointer to the added roomba
    void createRoomba(PoiQGraphicsEllipseItem *startPoint, bool virtualRoomba);

    //set's the MapQGraphicsView for this class. not done in constructor
    //because map is created later (needs this class).
    void setMap(MapQGraphicsView* map);
    void addPoi(PoiQGraphicsEllipseItem* poi);
    void addWall(WallQGraphicsLineItem* wall);
    std::set<WallQGraphicsLineItem*> getWalls();
    std::set<PoiQGraphicsEllipseItem *> getPOIs();
    void removePoi(PoiQGraphicsEllipseItem* poi);
    void removeWall(WallQGraphicsLineItem* wall);
    //removes all selected objects except roombas and startPoints
    void removeRedObjects();
    void removeAllObjects();
    void ifShowTraces();
    void removeTraces();
    void go2Pois();
    void stopFleet();

    //these functions work for selectedRoombas_

    //returns true if vertice at point is too close to a wall
    bool isBlocked(QPointF* point);
    //removes all blocked POIs and returns true if there was any.
    //This is called when creating a new wall
    bool removeBlockedPois();
    void connect(std::string stdip);
    void disconnect();
    void clean();
    void allMotorsOn();
    void allMotorsOff();
    void safeMode();
    void fullMode();
    void resetAngle();
    void playSong(unsigned int songNum);
    void setVelocity(int velocity);
    void drive( int velocity, int radius );
    void drive( int velocity);
    void correctLocation(Util::Direction direction);
    void correctAngle(bool clockWise);

    ~FleetManager();

signals:

public slots:
    void updateTimerTimeout();

private:

    //returns false is no roomba is ready to
    //go to a POI
    bool go2Poi(PoiQGraphicsEllipseItem *poi);

    MainWindow* mainWindow_;
    QVector<Croi::IRoomba*> selectedRoombas_;
    QVector<Croi::IRoomba*> managedRoombas_;
    QVector<Croi::IRoomba*> roombas_;
    std::set<WallQGraphicsLineItem*> walls_;
    std::set<PoiQGraphicsEllipseItem*> pois_;
    MapQGraphicsView* map_;
    QVector<QVector<Util::Vertice*>> vertices_;
    QTimer* updateTimer_;
};

#endif // FLEETMANAGER_H
