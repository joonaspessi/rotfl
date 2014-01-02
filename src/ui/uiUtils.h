#ifndef UIUTILS_H
#define UIUTILS_H

#include <limits>
#include <QGraphicsItem>

namespace Util {

// Defines how much above the mouse cursor the wall length hint is drawn, so it doesn't collide with the cursor
const unsigned int WALLLENGTHINDICATOROFFSET = 15;

typedef enum {
    CURSOR,
    WALL,
    POI,
    START
}SelectedPaintTool;

enum Direction {N, NE, E, SE, S, SW, W, NW};

const unsigned int VERTICEWIDTH = 10;
const unsigned int ROOMBAWIDTH = 34;
const unsigned int MAPWIDTH = 400;
const double ONTHEFLYCORRECTION = 2.0;

const int POITYPE = QGraphicsItem::UserType;
const int WALLTYPE = QGraphicsItem::UserType+1;

struct Vertice
{
    Vertice():
        topLeftX(0), topLeftY(0), n(NULL), ne(NULL), e(NULL), se(NULL),
        s(NULL), sw(NULL), w(NULL), nw(NULL), from(NULL), hasWall(false),
        blocked(false)
    {
        pos.setX(static_cast<double>(topLeftX)+Util::VERTICEWIDTH/2.0);
        pos.setY(static_cast<double>(topLeftY)+Util::VERTICEWIDTH/2.0);
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
