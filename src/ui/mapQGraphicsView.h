#ifndef MAPQGRAPHICSVIEW_H
#define MAPQGRAPHICSVIEW_H

#include <QGraphicsView>
#include "wallQGraphicsLineItem.h"
#include "uiUtils.h"
#include <QMouseEvent>
#include "croi/iRoomba.h"
//#include "fleetManager.h"

class FleetManager;

class MapQGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MapQGraphicsView(FleetManager* fleetManager, QWidget* parent = NULL);
    //gives map's width in cm
    unsigned int getMapWidth();
    //give new width in cm
    void setMapWidth(int width);
    void setSelectedPaintTool(Util::SelectedPaintTool tool);

    virtual ~MapQGraphicsView();

signals:
    void mapChanged();

public slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    WallQGraphicsLineItem* wallToBeAdded_;
    QPointF* wallToBeAddedStartPoint_;
    QGraphicsSimpleTextItem* wallToBeAddedStartPointText_;
    QGraphicsSimpleTextItem* wallToBeAddedEndPointText_;
    unsigned int mapWidth_;
    bool traceShown_;
    Util::SelectedPaintTool selectedPaintTool_;
    FleetManager* fleetManager_;
};

#endif // MAPQGRAPHICSVIEW_H
