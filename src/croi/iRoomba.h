#ifndef IROOMBA_H
#define IROOMBA_H

#include <exception>
#include <queue>
#include <string>
#include <QGraphicsPolygonItem>
#include <QObject>
#include <QPointF>
#include <QStack>
#include "atcQGraphicsRectItem.h"
#include "poiQGraphicsEllipseItem.h"
#include "uiUtils.h"

class MapQGraphicsView;
class FleetManager;

namespace Croi {

class IRoomba: public QObject
{
    Q_OBJECT

public:

    explicit IRoomba(PoiQGraphicsEllipseItem *startPoint,
                     MapQGraphicsView* map, FleetManager *parent = NULL);
    virtual ~IRoomba();

    virtual int rmb_connect(std::string ip) = 0;
    //subclass calls this implementation for setting isReady_ = false
    virtual int disconnect();
    //subclass calls this implementation for setting isReady_ = true
    virtual void safeMode();
    //subclass calls this implementation for setting isReady_ = true
    virtual void fullMode();
    virtual void allMotorsOn() = 0;
    virtual void allMotorsOff() = 0;
    virtual void clean();
    //subclass calls this implementation for setting velocity_ and radius_
    virtual void drive( int velocity, int radius );
    //subclass calls this implementation for setting velocity_
    virtual void drive( int velocity);
    virtual void playSong( int songNumber ) = 0;
    virtual char getTemperature() = 0;
    virtual unsigned short getChargeLevel() = 0;
    //this should only be called in updateState, because it only has new
    //value when updateState is called in sensorUpdateTimerTimeout
    virtual double getDistance() = 0;
    virtual double getTotalDistance();
    //this should only be called in updateState, because it only has new
    //value when updateState is called in sensorUpdateTimerTimeout
    virtual double getAngle() = 0;
    virtual bool getLeftBumb() = 0;
    virtual bool getRightBumb() = 0;
    //the basic implementation sets a real roomba icon
    virtual QGraphicsPixmapItem* setIcon();
    virtual void  goDock() = 0;
    virtual float getBatteryLevel()=0;

    //all functions below do not need reimplementation by subclasses
    void resetAngle();
    void correctLocation(Util::Direction direction);
    void correctAngle(bool clockWise);
    QPointF getLoc();  //location IN PIXELS is given as a point
    double getCurrentAngle();  //gives angle_
    int getRadius();  //gives radius_
    int getVelocity();
    MapQGraphicsView* getMap();
    //this function updates the class variables to represent actual roomba.
    //It's called in regular intervals in timer timeouts.
    void updateState();
    PoiQGraphicsEllipseItem* getStartPoint();
    void setStartPoint(PoiQGraphicsEllipseItem* startPoint);
    PoiQGraphicsEllipseItem* getDestPoi();
    //TODO: changed when implementing new roomba icon
    QGraphicsPixmapItem *getIcon();
    //calling this will show/unshow traces depending on whether
    //they'recurrently shown
    void ifShowTraces();
    void removeTraces();
    void drawTrace(double x, double y, Qt::GlobalColor color);
    //is roomba ready to receive drive commands
    bool isReady();
    //calculates the nearest path to point and returns it's distance
    double calcPath(QVector<QVector<Util::Vertice *> > &vertices, QPointF point);
    double calcPath(QVector<QVector<Util::Vertice *> > &vertices, PoiQGraphicsEllipseItem *poi);
    //this is a function for going straight to a point. Used by usePath
    void go2Point(QPointF point);
    //this makes the IRoomba to follow the path_ calculated by calcPath. This
    //must be called once and only once after calcPath if ignorePath isn't
    //called
    void usePath();
    //this makes the IRoomba to delete its path_. This should be called once
    //after calcPath if usePath isn't called. This can be used to make sure
    //that path_ is NULL
    void ignorePath();
    //Roomba stops moving and everything related to following a path is reset
    void stop();

	void setSquare(int squareWidth, int squareHeight);
    void setDestAtc(AtcQGraphicsRectItem* atc);

signals:
    void stateUpdate();
	void areaCleaned();

public slots:
    //updateState is now called. Also, if IRoomba is following a path, it
    //gives new go2Point() calls and updates bools as necessary
    void sensorUpdateTimerTimeout();

private slots:

    void turnTimerTimeout();
    void driveTimerTimeout();
    void squareTurn();
    void rotateEnded();
    void squareMoveOneLine();
    void squareTurn2();
    void squareMoveLong();
    void squareTurn3();
    void squareMoveOneLine2();
    void squareTurn4();
    void squareStart2();

private:
    QPointF calcLoc();  //X and Y coordinates are updated
    //function for comparing vertices
    static bool verticeCompare(Util::Vertice* first, Util::Vertice* second);
    //function for dealing with vertice's neighbour in Dijkstra's algorithm
    void compNeigh(Util::Vertice *curV, Util::Direction direction,
                   std::priority_queue<Util::Vertice *,
                                       std::vector<Util::Vertice*>,
                                       bool (*)(Util::Vertice*, Util::Vertice*)> &priQ);

    PoiQGraphicsEllipseItem *startPoint_;
    MapQGraphicsView *map_;
    //ICON
    QGraphicsPixmapItem *icon_;

    //roombas traces are shown here
    QVector<QGraphicsLineItem*> traces_;
    QVector<QGraphicsLineItem*> ctraces_;  //cleaned traces
    bool traceShown_;
    //current location's x-component IN PIXELS
    double Xloc_;
    //current location's y-component IN PIXELS
    double Yloc_;
    //current angle. Value range 0..2*PI
    double angle_;
    int radius_;
    int velocity_;
    bool isReady_;
    int driveTime_;
    QStack<QPointF> path_;
    //the path's lines are stored here as long as path_ != NULL
    QVector<QGraphicsLineItem*> pathLines_;
    bool followingPath_; //true when path_ is followed
    bool prevPReached_; //true briefly as last point in path is reached
    PoiQGraphicsEllipseItem *destPoi_;
    AtcQGraphicsRectItem* destAtc_;

    double totalDistance_;

	QTimer* drawTraceTimer;
    bool cleaning_;

    //for Square size
    void squareStart();
    int m_sx; //the length of one line in real world coordinates
    int m_sy;  //long move, ie. the Width of the rectangle in real world coordinates.
    int m_count;
    int nOfRound; // num of round, Height of rectangel is nOfRound*2*m_sx
    float ld;   //distance between lines in squares
    int countDown; //counting down number of round left

    double turnTime_;      //delta time from go2point, needed by cleaningATC.
    int turnDirection_; //directions that go2point used.
    void calc4square(int h);
   // void angleCorrection();
};


} //namespace CROI


#endif // IROOMBA_H
