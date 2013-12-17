#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QGraphicsScene>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QPushButton>
#include "croi/posixSerial.h"
#include "croi/iRoomba.h"
#include "mapQGraphicsView.h"
#include "uiUtils.h"
//#include "fleetManager.h"

class ThreadReader;
class FleetManager;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setRoombaStatusData(Croi::IRoomba* selectedRoomba);

    void setCurrentFile(const QString &fileName);
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *object, QEvent *event);
public slots:

    void mapModified();
private slots:
    void pushButton_removeRedObjects_clicked();

    void pushButton_Connect_clicked();

    void pushButton_Disconnect_clicked();

    void pushButton_Clean_clicked();

    void pushButton_Safe_clicked();

    void pushButton_Full_clicked();

    void pushButton_allMotorsOn_clicked();

    void pushButton_allMotorsOff_clicked();

    void pushButton_playSong_clicked();

    void pushButton_unshowTraces_clicked();

    void pushButton_mapWidth_clicked();

    void pushButton_resetAngle_clicked();

    void velocity_horizontalSlider_sliderMoved(int position);

    void pushButton_Go2POIs_clicked();

    void pushButton_stopFleet_clicked();

    void action_Cursor_toggled(bool toggleStatus);

    void action_Wall_toggled(bool toggleStatus);

    void action_Poi_toggled(bool toggleStatus);

    void action_Start_toggled(bool toggleStatus);
    void actionSave_triggered();
    void actionSaveAs_triggered();
    void actionOpen_triggered();
signals:
    
protected:
    void keyPressEvent(QKeyEvent *event);
private:
    void init();
    void saveToFile(QString &fileName);
    void openFile(const QString &fileName);
    Ui::MainWindow *ui;
    Croi::PosixSerial* posixserial;
    ThreadReader *threadReader;
    FleetManager* fleetManager_;
    QTimer *updateSensorData_;
    MapQGraphicsView* map_;
    void createConnectDock();
    void createActionDock();
    void createStatusDock();
    void createMapTestingDock();
    void createToolbar();
    QLineEdit *ipLineEdit_1_;
    QLineEdit *ipLineEdit_2_;
    QLineEdit *ipLineEdit_3_;
    QLineEdit *ipLineEdit_4_;
    QLabel *velocityValue_label_;
    QLabel *temperature_label_;
    QLabel *chargeLevel_label_;
    QLabel *rmbPosition_label_;
    QSlider *velocity_horizontalSlider_;
    QLineEdit *mapWidth_lineEdit_;
    QDockWidget *connection_dockWidget_;
    QDockWidget *status_dockWidget_;
    QDockWidget *action_dockWidget_;
    QDockWidget *mapTesting_dockWidget_;
    QToolBar *toolbar_;
    QString currentFile_;
    QObject *roombaStatus_;
    bool saveUnsavedChanges();
};

#endif // MAINWINDOW_H
