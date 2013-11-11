#ifndef FLEETMANAGER_H
#define FLEETMANAGER_H

#include "mainwindow.h"
#include <QObject>
#include <croi/iRoomba.h>
#include "mapQGraphicsView.h"
#include "wallQGraphicsLineItem.h"
#include "poiQGraphicsEllipseItem.h"
#include <set>
#include <string>

const int RADSTRAIGHT = 32768;
const int RADTURNCW = 65535;
const int RADTURNCCW = 1;

class FleetManager : public QObject
{
    Q_OBJECT
public:
    explicit FleetManager(MainWindow* mainWindow, QObject *parent = NULL);

    //makes new roomba, adds it to class variables and
    //also returns a pointer to the added roomba
    void createRoomba(PoiQGraphicsEllipseItem *startPoint);

    void setMap(MapQGraphicsView* map);
    void addPoi(PoiQGraphicsEllipseItem* poi);
    void addWall(WallQGraphicsLineItem* wall);
    void removePoi(PoiQGraphicsEllipseItem* poi);
    void removeRedObjects();
    void ifShowTraces();
    void removeTraces();
    void go2Poi();
    //removes POIs if it colllides with any item
    //TODO: exclude trace
    void checkPoiCollision();

    void connect(std::string stdip);
    void disconnect();
    void clean();
    void allMotorsOn();
    void allMotorsOff();
    void safeMode()   ;
    void fullMode();
    void resetAngle();
    void playSong(unsigned int songNum);
    void setVelocity(int velocity);
    void drive( int velocity, int radius );
    void drive( int velocity);

    ~FleetManager();

signals:

public slots:
//    Move to private when Roowifi AutoCapture removed
    void sensorUpdateTimerTimeout();

private:

    MainWindow* mainWindow_;
    Croi::IRoomba* selectedRoomba_;
    QVector<Croi::IRoomba*> roombas_;
    std::set<WallQGraphicsLineItem*> walls_;
    std::set<PoiQGraphicsEllipseItem*> pois_;
    MapQGraphicsView* map_;

};

#endif // FLEETMANAGER_H
