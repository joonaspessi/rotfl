#include <QListView>
#include <QDebug>
#include <QString>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <QDateTime>
#include <QGridLayout>
#include <QDockWidget>
#include <QHBoxLayout>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qfunctions_wince.h>
#include "threadReader.h"

#include "croi/iRoomba.h"
#include "croi/roombaSerial.h"
#include "croi/roombaRoowifi.h"
#include "croi/croiUtil.h"
#include "mapQGraphicsView.h"

MainWindow::MainWindow(QWidget *parent) :
    moving_(false), QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    posixserial = new Croi::PosixSerial();

//    Disabled until Roowifi AutoCapture is used instead
//    updateSensorData_ = new QTimer(this);
//    connect(updateSensorData_,SIGNAL(timeout()),this,SLOT(sensorUpdateTimerTimeout()));

    iRoomba_ = new Croi::RoombaRoowifi(this);
//    iRoomba_ = new Croi::RoombaSerial();

    //threadReader = new ThreadReader(posixserial, this);
    //threadReader->start();

    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));

    QAction* quitAct = new QAction(tr("&Quit"),this);
    quitAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    connect(quitAct, SIGNAL(triggered()),this,SLOT(close()));
    fileMenu->addAction(quitAct);

    // TODO: Magic fixed size for mainwindow
    resize(750,450);
    scene_ = new QGraphicsScene(QRect(0,0,398,398), this);
    mapQGraphicsView_ = new mapQGraphicsView(this);
    mapQGraphicsView_->setScene(scene_);
    mapQGraphicsView_->centerOn(0,0);
    mapQGraphicsView_->setMapWidth(398);
    setCentralWidget(mapQGraphicsView_);
    centralWidget()->setFixedWidth(400);
    centralWidget()->setFixedHeight(400);

    qDebug() << "children width: " << mapQGraphicsView_->childrenRect().width();
    qDebug() << "children height: " << mapQGraphicsView_->childrenRect().height();

    createConnectDock();
    createActionDock();
    createStatusDock();
    createMapTestingDock();
    tabifyDockWidget(status_dockWidget_,action_dockWidget_);
    tabifyDockWidget(action_dockWidget_,mapTesting_dockWidget_);
    tabifyDockWidget(mapTesting_dockWidget_,connection_dockWidget_);

    //show the default real world width of map in cm
    mapWidth_lineEdit_->setText(QString::number(mapQGraphicsView_->giveMapWidth()));
    // TODO: Height in this one?
    //show the default real world width of map in cm
//    mapWidth_lineEdit_->setText(QString::number(mapQGraphicsView_->giveMapWidth()));
}

MainWindow::~MainWindow()
{
    iRoomba_->disconnect();
    releaseKeyboard();
    delete ui;
}

Ui::MainWindow* MainWindow::giveUiPointer()
{
    return ui;
}

void MainWindow::createConnectDock()
{
    QVBoxLayout *connect_layout = new QVBoxLayout;

    QHBoxLayout *ipLineEdit_layout = new QHBoxLayout;
    ipLineEdit_1_ = new QLineEdit("192");
    ipLineEdit_2_ = new QLineEdit("168");
    ipLineEdit_3_ = new QLineEdit("1");
    ipLineEdit_4_ = new QLineEdit("145");
    ipLineEdit_layout->addWidget(ipLineEdit_1_);
    ipLineEdit_layout->addWidget(ipLineEdit_2_);
    ipLineEdit_layout->addWidget(ipLineEdit_3_);
    ipLineEdit_layout->addWidget(ipLineEdit_4_);

    QPushButton *connect_pushButton = new QPushButton("&Connect", this);
    connect(connect_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_Connect_clicked()));
    QPushButton * disconnect_pushButton = new QPushButton("&Disconnect", this);
    connect(disconnect_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_Disconnect_clicked()));
    connect_layout->addWidget(connect_pushButton);
    connect_layout->addWidget(disconnect_pushButton);
    connect_layout->addLayout(ipLineEdit_layout);

    connection_dockWidget_ = new QDockWidget(tr("Connection"), this);
    QWidget *connectionWidget = new QWidget;
    connectionWidget->setLayout(connect_layout);
    connection_dockWidget_->setWidget(connectionWidget);
    addDockWidget(Qt::BottomDockWidgetArea, connection_dockWidget_);
}

void MainWindow::createActionDock()
{
    QVBoxLayout *action_layout = new QVBoxLayout;
    QPushButton *clean_pushButton = new QPushButton("Cl&ean", this);
    connect(clean_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_Clean_clicked()));
    QPushButton *safe_pushButton = new QPushButton("&Safe", this);
    connect(safe_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_Safe_clicked()));
    QPushButton *full_pushButton = new QPushButton("F&ull", this);
    connect(full_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_Full_clicked()));
    QPushButton *motorsOn_pushButton = new QPushButton("&Motors on", this);
    connect(motorsOn_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_allMotorsOn_clicked()));
    QPushButton *motorsOff_pushButton = new QPushButton("Motors &off", this);
    connect(motorsOff_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_allMotorsOff_clicked()));
    QPushButton *playSong_pushButton = new QPushButton("&Play song", this);
    connect(playSong_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_playSong_clicked()));
    QPushButton *Go2POI_pushButton = new QPushButton("&Go 2 POI", this);
    connect(Go2POI_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_Go2POI_clicked()));
    velocity_horizontalSlider_ = new QSlider(Qt::Horizontal);
    velocity_horizontalSlider_->setMaximum(500);
    velocity_horizontalSlider_->setMinimum(-500);
    connect(velocity_horizontalSlider_,SIGNAL(valueChanged(int)),this,SLOT(velocity_horizontalSlider_sliderMoved(int)));

    action_layout->addWidget(clean_pushButton);
    action_layout->addWidget(safe_pushButton);
    action_layout->addWidget(full_pushButton);
    action_layout->addWidget(motorsOn_pushButton);
    action_layout->addWidget(motorsOff_pushButton);
    action_layout->addWidget(playSong_pushButton);
    action_layout->addWidget(Go2POI_pushButton);
    action_layout->addWidget(velocity_horizontalSlider_);

    QWidget *actionWidget = new QWidget;
    actionWidget->setLayout(action_layout);
    action_dockWidget_ = new QDockWidget(tr("Action"), this);
    action_dockWidget_->setWidget(actionWidget);
    addDockWidget(Qt::BottomDockWidgetArea, action_dockWidget_);
//    action_dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
}

void MainWindow::createStatusDock()
{
    QVBoxLayout *status_layout = new QVBoxLayout;

    QHBoxLayout *temperature_layout = new QHBoxLayout;
    temperature_label_ = new QLabel("0");
    QLabel *temperatureUnit_label = new QLabel("C");
    temperature_layout->addWidget(temperature_label_);
    temperature_layout->addWidget(temperatureUnit_label);

    QHBoxLayout *chargeLevel_layout = new QHBoxLayout;
    chargeLevel_label_ = new QLabel("0");
    QLabel *chargeLevelUnit_label = new QLabel("mAh");
    chargeLevel_layout->addWidget(chargeLevel_label_);
    chargeLevel_layout->addWidget(chargeLevelUnit_label);

    QHBoxLayout *velocity_layout = new QHBoxLayout;
    velocityValue_label_ = new QLabel("0");
    QLabel *velocityUnit_label = new QLabel("mm/s");
    velocity_layout->addWidget(velocityValue_label_);
    velocity_layout->addWidget(velocityUnit_label);

    status_layout->addLayout(temperature_layout);
    status_layout->addLayout(chargeLevel_layout);
    status_layout->addLayout(velocity_layout);

    status_dockWidget_ = new QDockWidget(tr("Status"), this);
    QWidget *statusWidget = new QWidget;
    statusWidget->setLayout(status_layout);
    status_dockWidget_->setWidget(statusWidget);
    addDockWidget(Qt::RightDockWidgetArea, status_dockWidget_);
}

void MainWindow::createMapTestingDock()
{
    QVBoxLayout *mapTesting_layout = new QVBoxLayout;
    QPushButton *removeRedObjects_pushButton = new QPushButton("Remove red objects", this);
    connect(removeRedObjects_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_removeRedObjects_clicked()));
    QPushButton *unshowTraces_pushButton = new QPushButton("(Un)show traces", this);
    connect(unshowTraces_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_unshowTraces_clicked()));
    QPushButton *simMov_pushButton = new QPushButton("Simulate movement", this);
    connect(simMov_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_simMov_clicked()));
    QPushButton *resetAngle_pushButton = new QPushButton("Reset angle", this);
    connect(resetAngle_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_resetAngle_clicked()));

    QHBoxLayout *mapWidth_layout = new QHBoxLayout;
    QLabel *mapWidth_label = new QLabel("Define map's width (cm):");
    mapWidth_lineEdit_ = new QLineEdit("");
    QPushButton *mapWidth_pushButton = new QPushButton("Ok", this);
    connect(mapWidth_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_mapWidth_clicked()));
    mapWidth_layout->addWidget(mapWidth_label);
    mapWidth_layout->addWidget(mapWidth_lineEdit_);
    mapWidth_layout->addWidget(mapWidth_pushButton);

    mapTesting_layout->addWidget(removeRedObjects_pushButton);
    mapTesting_layout->addWidget(unshowTraces_pushButton);
    mapTesting_layout->addWidget(simMov_pushButton);
    mapTesting_layout->addWidget(resetAngle_pushButton);
    mapTesting_layout->addLayout(mapWidth_layout);

    QWidget *mapTestingWidget = new QWidget;
    mapTestingWidget->setLayout(mapTesting_layout);
    mapTesting_dockWidget_ = new QDockWidget(tr("Map testing"), this);
    mapTesting_dockWidget_->setWidget(mapTestingWidget);
    addDockWidget(Qt::RightDockWidgetArea, mapTesting_dockWidget_);
//    action_dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
}

void MainWindow::pushButton_removeRedObjects_clicked()
{
    mapQGraphicsView_->removeRedObjects();
}

void MainWindow::pushButton_Connect_clicked()
{
//    Disabled until Roowifi AutoCapture is used instead
//    updateSensorData_->start(500);
    QString ip = ipLineEdit_1_->text() + "." + ipLineEdit_2_->text() + "." + ipLineEdit_3_->text()
            + "." + ipLineEdit_4_->text();
    std::string stdip = ip.toStdString();
    iRoomba_->rmb_connect(stdip);
}

void MainWindow::pushButton_Disconnect_clicked()
{
//    Disabled until Roowifi AutoCapture is used instead
//    updateSensorData_->stop();
    iRoomba_->disconnect();
    temperature_label_->setText("0");
    chargeLevel_label_->setText("0");
    velocity_horizontalSlider_->setValue(0);
    velocityValue_label_->setText("0");
}

void MainWindow::pushButton_Clean_clicked()
{
    iRoomba_->clean();
}

void MainWindow::pushButton_allMotorsOn_clicked()
{
    iRoomba_->allMotorsOn();
}

void MainWindow::pushButton_allMotorsOff_clicked()
{
    iRoomba_->allMotorsOff();
}

void MainWindow::pushButton_Safe_clicked()
{
    grabKeyboard();
    iRoomba_->safeMode();
}

void MainWindow::pushButton_Full_clicked()
{
    grabKeyboard();
    iRoomba_->fullMode();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    //Remember following special radiuses
    //Straight = 32768 or 32767
    //Turn in place clockwise = 65535
    //Turn in place counter-clockwise = 1
    qDebug() << "KeyPress";
    if(event->key() == Qt::Key_W) {
        if (velocity_horizontalSlider_->value() < 0)
        {
            velocity_horizontalSlider_->setValue(velocity_horizontalSlider_->value()*-1);
        }
        else if (velocity_horizontalSlider_->value() == 0)
        {
            velocity_horizontalSlider_->setValue(100);
        }
        else
        {
            iRoomba_->Drive(velocity_horizontalSlider_->value(),32767);
        }
        radius_ = 32767;
        moving_ = true;
        qDebug() << "UpArrow";
    }
    else if(event->key() == Qt::Key_A) {
        iRoomba_->Drive(velocity_horizontalSlider_->value(),200);
        radius_ = 200;
        moving_ = true;
        qDebug() << "RightArrow";
     }
    else if(event->key() == Qt::Key_D) {
        iRoomba_->Drive(velocity_horizontalSlider_->value(),-200);
        radius_ = -200;
        moving_ = true;
        qDebug() << "LeftArrow";
    }
    else if(event->key() == Qt::Key_S) {
        if (velocity_horizontalSlider_->value() > 0)
        {
            velocity_horizontalSlider_->setValue(velocity_horizontalSlider_->value()*-1);
        }
        else if (velocity_horizontalSlider_->value() == 0)
        {
            velocity_horizontalSlider_->setValue(-100);
        }
        else
        {
            iRoomba_->Drive(velocity_horizontalSlider_->value(),32767);
        }
        radius_ = 32767;
        moving_ = true;
        qDebug() << "BackArrow";
    }
    else if(event->key() == Qt::Key_E) {
        iRoomba_->Drive(velocity_horizontalSlider_->value(),65535);
        radius_ = 65535;
        moving_ = true;
        qDebug() << "Turn clockwise";
    }
    else {
        releaseKeyboard();
        iRoomba_->Drive(0,32767);
        radius_ = 32767;
        moving_ = false;
        qDebug() << "Stop";
    }
}

void MainWindow::sensorUpdateTimerTimeout()
{
//    qDebug() << "sensorUpdateTimerTimeout";
    temperature_label_->setText( QString::number( ( unsigned char )( iRoomba_->getTemperature() ) ) );
    chargeLevel_label_->setText( QString::number( (unsigned short)( iRoomba_->getChargeLevel() ) ) );
    mapQGraphicsView_->updateLoc(iRoomba_->getDistance(), iRoomba_->getAngle(), iRoomba_->getRadius(),
                           iRoomba_->getVelocity());
}

void MainWindow::pushButton_playSong_clicked()
{
    iRoomba_->playSong(1);
}

void MainWindow::pushButton_unshowTraces_clicked()
{
    mapQGraphicsView_->ifShowTraces();
}

void MainWindow::pushButton_simMov_clicked()
{
    double distance = -rand()%500;
    double angle = -(rand()%90-rand()%90);
    //mapQGraphicsView_->updateLoc(distance, angle/3.05, static_cast<int>(2000*(360-angle)/360),
    //                       rand()%500);
    mapQGraphicsView_->updateLoc(-1000/3.05, 0, 1, rand()%500);  //simple version
}

void MainWindow::velocity_horizontalSlider_sliderMoved(int position)
{
    velocityValue_label_->setText(QString::number(position));
    if (moving_) {
        iRoomba_->Drive(position,radius_);
    }
}

void MainWindow::pushButton_mapWidth_clicked()
{
    mapQGraphicsView_->setMapWidth(mapWidth_lineEdit_->text().toInt());
}

void MainWindow::pushButton_resetAngle_clicked()
{
    mapQGraphicsView_->resetAngle();
}

void MainWindow::pushButton_Go2POI_clicked()
{
    QPointF poiCoordinate = mapQGraphicsView_->getNextPoi();
    QPointF roombaCoordinate = mapQGraphicsView_->getRoombasLocation();
    qDebug() << "POI coordinate x: " << poiCoordinate.x()
             << " , y: " << poiCoordinate.y();
    qDebug() << "Roomba coordinate x: " << roombaCoordinate.x()
             << " , y: " << roombaCoordinate.y();
    float deltaX = abs(roombaCoordinate.x() - poiCoordinate.x());
    float deltaY = abs(roombaCoordinate.y() - poiCoordinate.y());
    float angleRadian = atan2(deltaY, deltaX);
    //float anglePi = angleRadian*180 / PI;

    //stop
    iRoomba_->Drive(0,32767);
    radius_ = 32767;
    //moving_ = false;

    double initialAngle = mapQGraphicsView_->getCurrentAngle();



    //calculate


}
