#ifndef FLEETMANAGER_H
#define FLEETMANAGER_H

//#include "mainwindow.h"
#include <QObject>
#include <croi/iRoomba.h>
#include "mapQGraphicsView.h"
#include "wallQGraphicsLineItem.h"
#include "poiQGraphicsEllipseItem.h"
#include <set>
#include <string>
#include <QTimer>

const int RADSTRAIGHT = 32768;
const int RADTURNCW = 65535;
const int RADTURNCCW = 1;

class MainWindow;

class FleetManager : public QObject
{
    Q_OBJECT
public:
    explicit FleetManager(MainWindow* mainWindow, QObject *parent = NULL);

    //makes new roomba, adds it to class variables and
    //also returns a pointer to the added roomba
    void createRoomba(PoiQGraphicsEllipseItem *startPoint);

    //set's the MapQGraphicsView for this class. not done in constructor
    //because map is created later (needs this class).
    void setMap(MapQGraphicsView* map);
    void addPoi(PoiQGraphicsEllipseItem* poi);
    void addWall(WallQGraphicsLineItem* wall);
    void removePoi(PoiQGraphicsEllipseItem* poi);
    //removes all selected objects except roombas and startPoints
    void removeRedObjects();
    void ifShowTraces();
    void removeTraces();
    void go2Poi();
    //removes POIs if it colllides with any item
    //TODO: exclude trace
    void checkPoiCollision();
    //these functions work for current selectedRoomba_
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

    ~FleetManager();

signals:

public slots:
    void updateTimerTimeout();

private:
    bool isRoombaSelected();

    MainWindow* mainWindow_;
    Croi::IRoomba* selectedRoomba_;
    QVector<Croi::IRoomba*> roombas_;
    std::set<WallQGraphicsLineItem*> walls_;
    std::set<PoiQGraphicsEllipseItem*> pois_;
    MapQGraphicsView* map_;
    QTimer* updateTimer_;
};

#endif // FLEETMANAGER_H
