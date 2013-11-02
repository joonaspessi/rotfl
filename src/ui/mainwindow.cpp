#include <QListView>
#include <QDebug>
#include <QString>
#include <QTime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <QDateTime>
#include <QGridLayout>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QToolBar>
#include <QToolButton>
#include <QButtonGroup>
#include <cmath>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qfunctions_wince.h>
#include "threadReader.h"

#include "croi/iRoomba.h"
#include "croi/roombaSerial.h"
#include "croi/roombaRoowifi.h"
#include "croi/croiUtil.h"
#include "mapQGraphicsView.h"
#include "fleetManager.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //    posixserial = new Croi::PosixSerial();

    //    Disabled until Roowifi AutoCapture is used instead
    //    updateSensorData_ = new QTimer(this);
    //    connect(updateSensorData_,SIGNAL(timeout()),this,SLOT(sensorUpdateTimerTimeout()));

    //threadReader = new ThreadReader(posixserial, this);
    //threadReader->start();

    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));

    QAction* quitAct = new QAction(tr("&Quit"),this);
    quitAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    connect(quitAct, SIGNAL(triggered()),this,SLOT(close()));
    fileMenu->addAction(quitAct);

    // TODO: Magic fixed size for mainwindow
    resize(750,450);

    fleetManager_ = new FleetManager(this);
    map_ = new MapQGraphicsView(fleetManager_, this);
    map_->setScene(new QGraphicsScene(QRect(0,0,398,398), this));
    map_->centerOn(0,0);
    map_->setMapWidth(398);
    fleetManager_->setMap(map_);
    setCentralWidget(map_);
    centralWidget()->setFixedWidth(400);
    centralWidget()->setFixedHeight(400);

    //qDebug() << "children width: " << map_->childrenRect().width();
    //qDebug() << "children height: " << map_->childrenRect().height();

    createConnectDock();
    createActionDock();
    createStatusDock();
    createMapTestingDock();
    tabifyDockWidget(status_dockWidget_,action_dockWidget_);
    tabifyDockWidget(action_dockWidget_,mapTesting_dockWidget_);
    tabifyDockWidget(mapTesting_dockWidget_,connection_dockWidget_);

    createToolbar();

    //show the default real world width of map in cm
    mapWidth_lineEdit_->setText(QString::number(map_->giveMapWidth()));
    // TODO: Height in this one?
    //show the default real world width of map in cm
    //    mapWidth_lineEdit_->setText(QString::number(map_->giveMapWidth()));
}

MainWindow::~MainWindow()
{
    releaseKeyboard();
    delete ui;
    delete fleetManager_;
}

void MainWindow::setRoombaStatusData(Croi::IRoomba* selectedRoomba)
{
    temperature_label_->setText( QString::number( ( unsigned char )( selectedRoomba->getTemperature() ) ) );
    chargeLevel_label_->setText( QString::number( (unsigned short)( selectedRoomba->getChargeLevel() ) ) );
    QPointF rmbPosition = selectedRoomba->getLoc();
    rmbPosition_label_->setText( "(" + QString::number(rmbPosition.x()) + " , " + QString::number(rmbPosition.y()) + ")" );
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

    rmbPosition_label_ = new QLabel("(0 , 0)");

    status_layout->addLayout(temperature_layout);
    status_layout->addLayout(chargeLevel_layout);
    status_layout->addLayout(velocity_layout);
    status_layout->addWidget(rmbPosition_label_);

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
    mapTesting_layout->addWidget(resetAngle_pushButton);
    mapTesting_layout->addLayout(mapWidth_layout);

    QWidget *mapTestingWidget = new QWidget;
    mapTestingWidget->setLayout(mapTesting_layout);
    mapTesting_dockWidget_ = new QDockWidget(tr("Map testing"), this);
    mapTesting_dockWidget_->setWidget(mapTestingWidget);
    addDockWidget(Qt::RightDockWidgetArea, mapTesting_dockWidget_);
    //    action_dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
}

void MainWindow::createToolbar()
{
    toolbar_ = new QToolBar(this);
    toolbar_->setMovable(false);
    toolbar_->setFloatable(false);

    QActionGroup *actionGroup = new QActionGroup(this);

    QAction* cursor_action = new QAction("Cursor", actionGroup);
    cursor_action->setIcon(QIcon(":icons/graphics/wall"));
    cursor_action->setCheckable(true);
    cursor_action->setChecked(true);
    map_->setSelectedPaintTool(Util::SelectedPaintTool::CURSOR);
    connect(cursor_action,SIGNAL(toggled(bool)),this,SLOT(action_Cursor_toggled(bool)));
    toolbar_->addAction(cursor_action);

    QAction* wall_action = new QAction("Wall", actionGroup);
    wall_action->setIcon(QIcon(":icons/graphics/wall"));
    wall_action->setCheckable(true);
    connect(wall_action,SIGNAL(toggled(bool)),this,SLOT(action_Wall_toggled(bool)));
    toolbar_->addAction(wall_action);

    QAction* poi_action = new QAction("Poi", actionGroup);
    poi_action->setIcon(QIcon(":icons/graphics/wall"));
    poi_action->setCheckable(true);
    connect(poi_action,SIGNAL(toggled(bool)),this,SLOT(action_Poi_toggled(bool)));
    toolbar_->addAction(poi_action);

    QAction* start_action = new QAction("Start", actionGroup);
    start_action->setIcon(QIcon(":icons/graphics/wall"));
    start_action->setCheckable(true);
    connect(start_action,SIGNAL(toggled(bool)),this,SLOT(action_Start_toggled(bool)));
    toolbar_->addAction(start_action);

    toolbar_->adjustSize();
    this->addToolBar(toolbar_);
}

void MainWindow::pushButton_removeRedObjects_clicked()
{
    fleetManager_->removeRedObjects();
}

void MainWindow::pushButton_Connect_clicked()
{
    QString ip = ipLineEdit_1_->text() + "." + ipLineEdit_2_->text() + "." + ipLineEdit_3_->text()
            + "." + ipLineEdit_4_->text();
    std::string stdip = ip.toStdString();
    fleetManager_->connect(stdip);
}

void MainWindow::pushButton_Disconnect_clicked()
{
    //    Disabled until Roowifi AutoCapture is used instead
    //    updateSensorData_->stop();
    fleetManager_->disconnect();
    temperature_label_->setText("0");
    chargeLevel_label_->setText("0");
    velocity_horizontalSlider_->setValue(0);
    velocityValue_label_->setText("0");
}

void MainWindow::pushButton_Clean_clicked()
{
    fleetManager_->clean();
}

void MainWindow::pushButton_allMotorsOn_clicked()
{
    fleetManager_->allMotorsOn();
}

void MainWindow::pushButton_allMotorsOff_clicked()
{
    fleetManager_->allMotorsOff();
}

void MainWindow::pushButton_Safe_clicked()
{
    grabKeyboard();
    fleetManager_->safeMode();
}

void MainWindow::pushButton_Full_clicked()
{
    grabKeyboard();
    fleetManager_->fullMode();
}

void MainWindow::pushButton_resetAngle_clicked()
{
    fleetManager_->resetAngle();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
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
            fleetManager_->drive(velocity_horizontalSlider_->value(), RADSTRAIGHT);
        }
        qDebug() << "UpArrow";
    }
    else if(event->key() == Qt::Key_A) {
        fleetManager_->drive(velocity_horizontalSlider_->value(),200);
        qDebug() << "RightArrow";
    }
    else if(event->key() == Qt::Key_D) {
        fleetManager_->drive(velocity_horizontalSlider_->value(),-200);
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
            fleetManager_->drive(velocity_horizontalSlider_->value(),RADSTRAIGHT);
        }
        qDebug() << "BackArrow";
    }
    else if(event->key() == Qt::Key_E) {
        fleetManager_->drive(velocity_horizontalSlider_->value(), RADTURNCW);
        qDebug() << "Turn clockwise";
    }
    else if(event->key() == Qt::Key_Q) {
        releaseKeyboard();
        fleetManager_->drive(0, RADSTRAIGHT);
        qDebug() << "Stop";
    }
}

void MainWindow::pushButton_playSong_clicked()
{
    fleetManager_->playSong(1);
}

void MainWindow::pushButton_unshowTraces_clicked()
{
    fleetManager_->ifShowTraces();
}

void MainWindow::velocity_horizontalSlider_sliderMoved(int position)
{
    velocityValue_label_->setText(QString::number(position));
    fleetManager_->drive(position);
}

void MainWindow::pushButton_mapWidth_clicked()
{
    map_->setMapWidth(mapWidth_lineEdit_->text().toInt());
}

void MainWindow::pushButton_Go2POI_clicked()
{
    fleetManager_->go2Poi();
}

void MainWindow::action_Cursor_toggled(bool toggleStatus)
{
    if (toggleStatus) {
        map_->setSelectedPaintTool(Util::SelectedPaintTool::CURSOR);
    }
}

void MainWindow::action_Wall_toggled(bool toggleStatus)
{
    if (toggleStatus) {
        map_->setSelectedPaintTool(Util::SelectedPaintTool::WALL);
    }
}

void MainWindow::action_Poi_toggled(bool toggleStatus)
{
    if (toggleStatus) {
        map_->setSelectedPaintTool(Util::SelectedPaintTool::POI);
    }
}

void MainWindow::action_Start_toggled(bool toggleStatus)
{
    if (toggleStatus) {
        map_->setSelectedPaintTool(Util::SelectedPaintTool::START);
    }
}

void MainWindow::pushButton_Go2POI_clicked()
{
    QPointF poiCoordinate = mapQGraphicsView_->getNextPoi();
    QPointF roombaCoordinate = mapQGraphicsView_->getRoombasLocation();
    qDebug() << "POI coordinate x: " << poiCoordinate.x()
             << " , y: " << poiCoordinate.y();
    qDebug() << "Roomba coordinate x: " << roombaCoordinate.x()
             << " , y: " << roombaCoordinate.y();

    //stop
    iRoomba_->Drive(0,32767);
    radius_ = 32767;
    moving_ = false;

    //calculate the angle

    float deltaX = poiCoordinate.x()-roombaCoordinate.x();
    float deltaY = roombaCoordinate.y() - poiCoordinate.y();
    //float angleRadian = atan2(deltaY, deltaX);
    //float anglePi = angleRadian*180 / PI;

    qDebug() << "Delta X: " << deltaX;
    qDebug() << "Delta Y: " << deltaY;

    float calculatedAngle = 0;

    if (deltaX == 0 && deltaY > 0)
    {
        calculatedAngle = -0.5*PI;
    }
    else
    {
        calculatedAngle = atan2(deltaY, deltaX);
        qDebug() << "Calculated angle in degrees: " << calculatedAngle*(180/PI);
    }
    /*
    else if ( deltaX>0 && deltaY <=0 )
    {
        //calculatedAngle = -atan2(deltaY, deltaX);
        calculatedAngle = atan2(deltaY, deltaX);
        qDebug() << "Calculated angle in degrees: " << calculatedAngle*(180/PI);
    }
    else if ( deltaX<=0 && deltaY <0 )
    {
        //calculatedAngle = PI/2+atan2(deltaX, deltaY);
        calculatedAngle = atan2(deltaY, deltaX);
        qDebug() << "Calculated angle in degrees: " << calculatedAngle*(180/PI);
    }
    else if ( deltaX<0 && deltaY >=0 )
    {
        //calculatedAngle = PI-atan2(deltaY,deltaX);
        calculatedAngle = atan2(deltaY, deltaX);
        qDebug() << "Calculated angle in degrees: " << calculatedAngle*(180/PI);
    }
    else if ( deltaX>0 && deltaY >0 )
    {
        //calculatedAngle = 2*PI-atan2(deltaY, deltaX);
        calculatedAngle = atan2(deltaY, deltaX);
        qDebug() << "Calculated angle in degrees: " << calculatedAngle*(180/PI);
    }
    else
    {
        qDebug() << "Calculating the turning angle failed";
    }*/

    float turningAngle = mapQGraphicsView_->getCurrentAngle() - calculatedAngle;
    int turningSpeed = 50;
    float turningTime = 0;

    qDebug() << "Turning angle in degrees: " << turningAngle*(180/PI);


    iRoomba_->Drive(velocity_horizontalSlider_->value(),65535);
    radius_ = 65535;
    moving_ = true;

    short dRoombaAngle = 0;

    QTime dieTime = QTime::currentTime().addSecs(5);
    while( true/*QTime::currentTime() < dieTime */) {

        dRoombaAngle += iRoomba_->getAngle();
        qDebug() << "Roomba sensor read angle " << dRoombaAngle;

        if(dRoombaAngle > (-3,05)*turningAngle*(180/PI)) break;

        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
    }

    iRoomba_->Drive(0,32767);
    radius_ = 32767;
    moving_ = false;


    if (turningAngle < 0) //Turn counter-clockwise
    {
        turningTime = -(((PI*TRACEWIDTH*10)/turningSpeed)/(2*PI))*turningAngle;
    }
    else //Turn clockwise
    {
        turningTime = (((PI*TRACEWIDTH*10)/turningSpeed)/(2*PI))*turningAngle;
    }

    qDebug() << "Turning time: " << turningTime;

}
