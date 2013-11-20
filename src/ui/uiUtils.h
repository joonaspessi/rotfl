#ifndef UIUTILS_H
#define UIUTILS_H

namespace Util {

// Defines how much above the mouse cursor the wall length hint is drawn, so it doesn't collide with the cursor
const unsigned int WALLLENGTHINDICATOROFFSET = 15;

typedef enum {
    CURSOR,
    WALL,
    POI,
    START
}SelectedPaintTool;

const unsigned int VERTICEWIDTH = 40;
const unsigned int MAPWIDTH = 400;
}

#endif // UIUTILS_H
