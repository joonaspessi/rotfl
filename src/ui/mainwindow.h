#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QGraphicsScene>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
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
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_pushButton_removeRedObjects_clicked();

    void on_pushButton_Connect_clicked();

    void on_pushButton_Disconnect_clicked();

    void on_pushButton_Clean_clicked();

    void on_pushButton_Safe_clicked();

    void on_pushButton_Full_clicked();

    void on_pushButton_allMotorsOn_clicked();

    void on_pushButton_allMotorsOff_clicked();

    void on_pushButton_playSong_clicked();

    void on_pushButton_simMov_clicked();

    void on_pushButton_unshowTraces_clicked();

    void on_pushButton_mapWidth_clicked();

    void on_pushButton_resetAngle_clicked();

    void on_velocity_horizontalSlider_sliderMoved(int position);

signals:
    
protected:
    void keyPressEvent(QKeyEvent *event);
private:
    void init();
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    int index;
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
