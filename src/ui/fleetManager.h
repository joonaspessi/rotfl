#ifndef FLEETMANAGER_H
#define FLEETMANAGER_H

#include <QObject>
#include <croi/iRoomba.h>
#include "wallQGraphicsLineItem.h"
#include "atcQGraphicsRectItem.h"
#include "poiQGraphicsEllipseItem.h"
#include <set>
#include <string>
#include <QVector>
#include <QTimer>
#include "math.h"

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
    void addAtc(AtcQGraphicsRectItem* atc);
    //adds the wall and removes POIs too close to the new wall
    //It's currently slow and should be optimized
    void addWall(WallQGraphicsLineItem* wall);
    std::set<WallQGraphicsLineItem*> getWalls();
    QVector<PoiQGraphicsEllipseItem *> getPOIs();
    std::set<AtcQGraphicsRectItem *> getATCs();
    void removePoi(PoiQGraphicsEllipseItem* poi);
    //removes the wall and updates vertices booleans hasWall and isBlocked
    //and takes into account situation with several walls near or intersecting
    void removeWall(WallQGraphicsLineItem* wall);
    void removeAtc(AtcQGraphicsRectItem* atc);
    //removes all selected objects except roombas and startPoints
    void removeRedObjects();
    void removeAllObjects();
    void ifShowTraces();  //tells the roombas to change the visibility of their trace
    void removeTraces();
    void go2Pois();  //roombas will collect all POIs that are possible
    void stopFleet(bool wallHit);  //stops all fleet management. wallHit == true outputs a warning message
    //called when IRoomba reaches a POI. Calculates the nearest POI for the roomba or ends the collecting
    //of POIs if all are collected or the rest are unreachable.
    void poiCollected(Croi::IRoomba *collector, PoiQGraphicsEllipseItem *poi);
    void atcCleaned(Croi::IRoomba *roomba);
    //these functions work for selectedRoombas_

    //returns true if vertice at point is too close to a wall
    bool isBlocked(QPointF* point);
    //removes all blocked POIs and returns true if there was any.
    //This is called when creating a new wall
    bool removeBlockedPois();
    void connect(std::string stdip);
    void disconnect();
    void goDock();
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
    int findNearestPoint(Croi::IRoomba *roomba);
    //returns false if there is no ATC left
    bool MoveRobotToNearestArea(int i);

    ~FleetManager();

signals:

public slots:
    void updateTimerTimeout();
    void clean();

private:

    //returns false is no roomba is ready to
    //go to a POI
    bool go2Poi(PoiQGraphicsEllipseItem *poi);

    MainWindow* mainWindow_;
    QVector<Croi::IRoomba*> selectedRoombas_;
    QVector<Croi::IRoomba*> managedRoombas_;
    QVector<Croi::IRoomba*> roombas_;
    std::set<WallQGraphicsLineItem*> walls_;
    QVector<PoiQGraphicsEllipseItem*> pois_;
	std::set<AtcQGraphicsRectItem*> atcs_;
    MapQGraphicsView* map_;
    QVector<QVector<Util::Vertice*>> vertices_;
    QTimer* updateTimer_;
    bool go2PoisOn_;
    bool cleaningAtcsOn_;
};

#endif // FLEETMANAGER_H
