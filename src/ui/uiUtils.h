#ifndef UIUTILS_H
#define UIUTILS_H

#include <limits>
#include <QGraphicsItem>
#include <cmath>

namespace Util {

// Defines how much above the mouse cursor the wall length hint is drawn, so it doesn't collide with the cursor
const unsigned int WALLLENGTHINDICATOROFFSET = 15;

typedef enum {
    CURSOR,
    WALL,
    ATC,
    POI,
    START,
    STARTVIRTUAL
}SelectedPaintTool;

enum Direction {N, NE, E, SE, S, SW, W, NW};

const unsigned int REALVERTICEWIDTH = 10;  //real world width
const unsigned int PIXELMAPWIDTH = 600;
const unsigned int REALMAPWIDTH = 800;
const unsigned int TABWIDTH = 700;
const double COORDCORRECTION = double(REALMAPWIDTH)/PIXELMAPWIDTH;
const unsigned int VERTICEWIDTH = ceil(REALVERTICEWIDTH/COORDCORRECTION);  //width in pixels
const unsigned int REALROOMBAWIDTH = 34;  //real world width in cm
const unsigned int ROOMBAWIDTH = round(REALROOMBAWIDTH/COORDCORRECTION);  //width in pixels
const unsigned int REALCLEANWIDTH = 20;  //real world width in cm
const unsigned int PIXELCLEANWIDTH = round(REALCLEANWIDTH/COORDCORRECTION);  //width in pixels

const int POITYPE = QGraphicsItem::UserType;
const int WALLTYPE = QGraphicsItem::UserType+1;
const int ATCTYPE = QGraphicsItem::UserType+2;
const int RADSTRAIGHT = 32768;
const int RADTURNCW = 65535;
const int RADTURNCCW = 1;
const double PI = 3.14159265;
const double POIWIDTH = 10.0;
const double TRACEWIDTH = ROOMBAWIDTH/4;
const double ONTHEFLYCORRECTION = 2.0;


struct Vertice
{
    Vertice():
        topLeftX(0), topLeftY(0), n(NULL), ne(NULL), e(NULL), se(NULL),
        s(NULL), sw(NULL), w(NULL), nw(NULL), from(NULL), hasWall(false),
        blocked(false)
    {
        pos.setX(double(topLeftX)+Util::VERTICEWIDTH/2.0);
        pos.setY(double(topLeftY)+Util::VERTICEWIDTH/2.0);
        dist = std::numeric_limits<double>::max();
    }

    //vertice's location and area
    unsigned int topLeftX;
    unsigned int topLeftY;
    QPointF pos;  //center point

    //neighbouring vertices. Will be NULL when contact is severed by a wall.
    Vertice* n;
    Vertice* ne;
    Vertice* e;
    Vertice* se;
    Vertice* s;
    Vertice* sw;
    Vertice* w;
    Vertice* nw;

    bool hasWall;  //when the vertice collides with wall
    bool blocked;  //as above or too close to a wall

    //needed for finding shortest path
    Vertice* from;
    double dist;
};

}

#endif // UIUTILS_H
