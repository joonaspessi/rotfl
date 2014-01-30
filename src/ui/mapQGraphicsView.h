#ifndef MAPQGRAPHICSVIEW_H
#define MAPQGRAPHICSVIEW_H

#include <QGraphicsView>
#include "wallQGraphicsLineItem.h"
#include "atcQGraphicsRectItem.h"
#include "uiUtils.h"
#include <QMouseEvent>
#include "../croi/iRoomba.h"

//#include "fleetManager.h"

class FleetManager;

class MapQGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MapQGraphicsView(FleetManager* fleetManager, QWidget* parent = NULL);
    //gives map's width in cm
    unsigned int getmapScale();
    //give new width in cm
    void setmapScale(double scaleFactor);
    void setSelectedPaintTool(Util::SelectedPaintTool tool);

    virtual ~MapQGraphicsView();

signals:
    void mapChanged();
    void roombaSelected();

public slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    AtcQGraphicsRectItem* atcToBeAdded_;
    WallQGraphicsLineItem* wallToBeAdded_;
    QPointF* wallToBeAddedStartPoint_;
    QPointF* atcToBeAddedStartPoint_;
    QGraphicsSimpleTextItem* wallToBeAddedStartPointText_;
    QGraphicsSimpleTextItem* wallToBeAddedEndPointText_;
	QGraphicsSimpleTextItem* atcToBeAddedStartPointText_;
    QGraphicsSimpleTextItem* atcToBeAddedEndPointText_;
    unsigned int mapScale_;
    bool traceShown_;
    Util::SelectedPaintTool selectedPaintTool_;
    FleetManager* fleetManager_;
};

#endif // MAPQGRAPHICSVIEW_H
