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

class ThreadReader;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Ui::MainWindow *giveUiPointer();

public slots:
//    Move to private when Roowifi AutoCapture removed
    void sensorUpdateTimerTimeout();

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

    void pushButton_simMov_clicked();

    void pushButton_unshowTraces_clicked();

    void pushButton_mapWidth_clicked();

    void pushButton_resetAngle_clicked();

    void velocity_horizontalSlider_sliderMoved(int position);

signals:
    
protected:
    void keyPressEvent(QKeyEvent *event);
private:
    void init();
    Ui::MainWindow *ui;
    Croi::PosixSerial* posixserial;
    ThreadReader *threadReader;
    bool moving_;
    int radius_;
    Croi::IRoomba* iRoomba_;
    QTimer *updateSensorData_;
    mapQGraphicsView* mapQGraphicsView_;
    QGraphicsScene* scene_;
    void createConnectDock();
    void createActionDock();
    void createStatusDock();
    void createMapTestingDock();
    QLineEdit *ipLineEdit_1_;
    QLineEdit *ipLineEdit_2_;
    QLineEdit *ipLineEdit_3_;
    QLineEdit *ipLineEdit_4_;
    QLabel *velocityValue_label_;
    QLabel *temperature_label_;
    QLabel *chargeLevel_label_;
    QSlider *velocity_horizontalSlider_;
    QLineEdit *mapWidth_lineEdit_;
    QDockWidget *connection_dockWidget_;
    QDockWidget *status_dockWidget_;
    QDockWidget *action_dockWidget_;
    QDockWidget *mapTesting_dockWidget_;
};

#endif // MAINWINDOW_H
