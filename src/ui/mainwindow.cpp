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
#include <QFileDialog>
#include <QMessageBox>
#include <QQuickView>
#include <QtQml>
#include <QtQuick>

#include <cmath>
#include <unistd.h>

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
#include "flogger.h"

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

    QAction* openAct = new QAction(tr("&Open"),this);
    openAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    connect(openAct, SIGNAL(triggered()),this,SLOT(actionOpen_triggered()));
    fileMenu->addAction(openAct);

    QAction* saveAct = new QAction(tr("&Save"),this);
    saveAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    connect(saveAct, SIGNAL(triggered()),this,SLOT(actionSave_triggered()));
    fileMenu->addAction(saveAct);

    QAction* saveAsAct = new QAction(tr("S&ave as"),this);
    saveAsAct->setShortcut(QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_S));
    connect(saveAsAct, SIGNAL(triggered()),this,SLOT(actionSaveAs_triggered()));
    fileMenu->addAction(saveAsAct);

    QAction* quitAct = new QAction(tr("&Quit"),this);
    quitAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    connect(quitAct, SIGNAL(triggered()),this,SLOT(close()));
    fileMenu->addAction(quitAct);

    // TODO: Magic fixed size for mainwindow
    resize(750,450);

    fleetManager_ = new FleetManager(this);
    map_ = new MapQGraphicsView(fleetManager_, this);
    map_->setScene(new QGraphicsScene(QRect(0,0,Util::MAPWIDTH,Util::MAPWIDTH), this));
    map_->centerOn(0,0);
    fleetManager_->setMap(map_);
    setCentralWidget(map_);
    centralWidget()->setFixedWidth(Util::MAPWIDTH+2);
    centralWidget()->setFixedHeight(Util::MAPWIDTH+2);

    //qDebug() << "children width: " << map_->childrenRect().width();
    //qDebug() << "children height: " << map_->childrenRect().height();

    createConnectDock();
    createActionDock();
    createStatusDock();
    createMapTestingDock();

    //QML
    QQuickView * qmlview = new QQuickView();
    QWidget *container = QWidget::createWindowContainer(qmlview,this);

    container->setMinimumSize(400,380);
    container->setMaximumSize(400,380);
    container->setFocusPolicy(Qt::TabFocus);
    qmlview->setSource(QUrl("qrc:/widgets/gauges"));

    QDockWidget *widgetti = new QDockWidget(tr("gauges"), this);
    widgetti->setWidget(container);
    addDockWidget(Qt::RightDockWidgetArea,widgetti);

     roombaStatus_ = qmlview->rootObject();
    //End of QML

    tabifyDockWidget(status_dockWidget_,action_dockWidget_);
    tabifyDockWidget(action_dockWidget_,mapTesting_dockWidget_);
    tabifyDockWidget(mapTesting_dockWidget_,connection_dockWidget_);
    tabifyDockWidget(connection_dockWidget_, widgetti);

    createToolbar();

    setCurrentFile("");

    // TODO: Improve this to NOT trigger on start
    connect(map_,SIGNAL(mapChanged()),this,SLOT(mapModified()));

    // This event filter is implemented in eventFilter function, keeps mouse coordinates in status bar
    qApp->installEventFilter(this);
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
    rmbPosition_label_->setText( "(" + QString::number(rmbPosition.x()*Util::COORDCORRECTION) +
                                 " , " + QString::number(rmbPosition.y()*Util::COORDCORRECTION) + ")" );

    //QML
    QVariant returnedValue;
    QMetaObject::invokeMethod(roombaStatus_, "setBatteryLevelmAh", Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, selectedRoomba->getChargeLevel()) );


    QMetaObject::invokeMethod(roombaStatus_, "setSpeed", Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, abs(selectedRoomba->getVelocity())));

    QMetaObject::invokeMethod(roombaStatus_, "setDirection", Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, abs(selectedRoomba->getCurrentAngle()*180/Util::PI)));

    QMetaObject::invokeMethod(roombaStatus_, "setTemperature", Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, abs(selectedRoomba->getTemperature())));

    QMetaObject::invokeMethod(roombaStatus_, "setDistance", Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, abs(selectedRoomba->getDistance())));

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
    QPushButton *Go2POIs_pushButton = new QPushButton("&Go 2 POIs", this);
    connect(Go2POIs_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_Go2POIs_clicked()));
    QPushButton *stopFleet_pushButton = new QPushButton("S&top Fleet", this);
    connect(stopFleet_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_stopFleet_clicked()));
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
    action_layout->addWidget(Go2POIs_pushButton);
    action_layout->addWidget(stopFleet_pushButton);
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
    QPushButton *correctLeft_pushButton = new QPushButton("Correct location to left by 2 cm", this);
    connect(correctLeft_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_correctLeft_clicked()));
    QPushButton *correctRight_pushButton = new QPushButton("Correct location to right by 2 cm", this);
    connect(correctRight_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_correctRight_clicked()));
    QPushButton *correctUp_pushButton = new QPushButton("Correct location to up by 2 cm", this);
    connect(correctUp_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_correctUp_clicked()));
    QPushButton *correctDown_pushButton = new QPushButton("Correct location to down by 2 cm", this);
    connect(correctDown_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_correctDown_clicked()));
    QPushButton *correctCw_pushButton = new QPushButton("Correct angle clockwise by 2 degrees", this);
    connect(correctCw_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_correctCw_clicked()));
    QPushButton *correctCcw_pushButton = new QPushButton("Correct angle counterclockwise by 2 degrees", this);
    connect(correctCcw_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_correctCcw_clicked()));

    QHBoxLayout *mapScale_layout = new QHBoxLayout;
    QLabel *mapScale_label = new QLabel("Zoom factor:");
    mapScale_horizontalSlider_ = new QSlider(Qt::Horizontal);
    mapScale_horizontalSlider_->setMinimum(10);
    mapScale_horizontalSlider_->setMaximum(50);
    connect(mapScale_horizontalSlider_,SIGNAL(valueChanged(int)),this,SLOT(mapScale_horizontalSlider_sliderMoved(int)));
    mapScaleValue_label_ = new QLabel("1.0");
    mapScale_layout->addWidget(mapScale_label);
    mapScale_layout->addWidget(mapScale_horizontalSlider_);
    mapScale_layout->addWidget(mapScaleValue_label_);

    mapTesting_layout->addWidget(removeRedObjects_pushButton);
    mapTesting_layout->addWidget(unshowTraces_pushButton);
    mapTesting_layout->addWidget(resetAngle_pushButton);
    mapTesting_layout->addWidget(correctLeft_pushButton);
    mapTesting_layout->addWidget(correctRight_pushButton);
    mapTesting_layout->addWidget(correctUp_pushButton);
    mapTesting_layout->addWidget(correctDown_pushButton);
    mapTesting_layout->addWidget(correctCw_pushButton);
    mapTesting_layout->addWidget(correctCcw_pushButton);
    mapTesting_layout->addLayout(mapScale_layout);

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
    cursor_action->setIcon(QIcon(":/icons/clear_hand"));
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
    poi_action->setIcon(QIcon(":/icons/poi"));
    poi_action->setCheckable(true);
    connect(poi_action,SIGNAL(toggled(bool)),this,SLOT(action_Poi_toggled(bool)));
    toolbar_->addAction(poi_action);

    QAction* start_action = new QAction("Start", actionGroup);
    start_action->setIcon(QIcon(":/icons/roomba_small"));
    start_action->setCheckable(true);
    connect(start_action,SIGNAL(toggled(bool)),this,SLOT(action_Start_toggled(bool)));
    toolbar_->addAction(start_action);

    QAction* startVirtual_action = new QAction("StartVirtual", actionGroup);
    startVirtual_action->setIcon(QIcon(":/icons/roomba_virtual_small"));
    startVirtual_action->setCheckable(true);
    connect(startVirtual_action,SIGNAL(toggled(bool)),this,SLOT(action_StartVirtual_toggled(bool)));
    toolbar_->addAction(startVirtual_action);

    toolbar_->adjustSize();
    this->addToolBar(toolbar_);
}

void MainWindow::pushButton_removeRedObjects_clicked()
{
    fleetManager_->removeRedObjects();
    (*flog.ts) << "Remove red objects Button pressed." << endl;
}

void MainWindow::pushButton_Connect_clicked()
{
    QString ip = ipLineEdit_1_->text() + "." + ipLineEdit_2_->text() + "." + ipLineEdit_3_->text()
            + "." + ipLineEdit_4_->text();
    std::string stdip = ip.toStdString();
    fleetManager_->connect(stdip);
    (*flog.ts) << "Connect Button pressed." << endl;
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
    (*flog.ts) << "Disconnect Button pressed." << endl;
}

void MainWindow::pushButton_Clean_clicked()
{
    fleetManager_->clean();
    (*flog.ts) << "Clean Button pressed." << endl;
}

void MainWindow::pushButton_allMotorsOn_clicked()
{
    fleetManager_->allMotorsOn();
    (*flog.ts) << "Motors on Button pressed." << endl;
}

void MainWindow::pushButton_allMotorsOff_clicked()
{
    fleetManager_->allMotorsOff();
    (*flog.ts) << "Motors off Button pressed." << endl;
}

void MainWindow::pushButton_Safe_clicked()
{
    grabKeyboard();
    fleetManager_->safeMode();
    (*flog.ts) << "Safe Button pressed." << endl;
}

void MainWindow::pushButton_Full_clicked()
{
    grabKeyboard();
    fleetManager_->fullMode();
    (*flog.ts) << "Full Button pressed." << endl;
}

void MainWindow::pushButton_resetAngle_clicked()
{
    fleetManager_->resetAngle();
    (*flog.ts) << "ResetAngle Button pressed." << endl;
}

void MainWindow::pushButton_correctLeft_clicked()
{
    fleetManager_->correctLocation(Util::Direction::W);
}

void MainWindow::pushButton_correctRight_clicked()
{
    fleetManager_->correctLocation(Util::Direction::E);
}

void MainWindow::pushButton_correctUp_clicked()
{
    fleetManager_->correctLocation(Util::Direction::N);
}

void MainWindow::pushButton_correctDown_clicked()
{
    fleetManager_->correctLocation(Util::Direction::S);
}

void MainWindow::pushButton_correctCw_clicked()
{
    fleetManager_->correctAngle(true);
}

void MainWindow::pushButton_correctCcw_clicked()
{
    fleetManager_->correctAngle(false);
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
            fleetManager_->drive(velocity_horizontalSlider_->value(), Util::RADSTRAIGHT);
        }
        qDebug() << "UpArrow";
        (*flog.ts) << "UpArrow" << endl;
    }
    else if(event->key() == Qt::Key_A) {
        fleetManager_->drive(velocity_horizontalSlider_->value(), Util::RADTURNCCW);
        qDebug() << "Turn counterclockwise";
        (*flog.ts) << "Turn counterclockwise" << endl;
    }
    else if(event->key() == Qt::Key_D) {
        fleetManager_->drive(velocity_horizontalSlider_->value(), Util::RADTURNCW);
        qDebug() << "Turn clockwise";
        (*flog.ts) << "Turn clockwise" << endl;
    }
    else if(event->key() == Qt::Key_E) {
        fleetManager_->drive(velocity_horizontalSlider_->value(),200);
        qDebug() << "RightArrow";
        (*flog.ts) << "RightArrow" << endl;
    }
    else if(event->key() == Qt::Key_R) {
        fleetManager_->drive(velocity_horizontalSlider_->value(),-200);
        qDebug() << "LeftArrow";
        (*flog.ts) << "LeftArrow" << endl;
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
            fleetManager_->drive(velocity_horizontalSlider_->value(),Util::RADSTRAIGHT);
        }
        qDebug() << "BackArrow";
        (*flog.ts) << "BackArrow" << endl;
    }
    else if(event->key() == Qt::Key_Q) {
        releaseKeyboard();
        fleetManager_->drive(0, Util::RADSTRAIGHT);
        qDebug() << "Stop";
        (*flog.ts) << "Stop" << endl;
    }
}

void MainWindow::pushButton_playSong_clicked()
{
    fleetManager_->playSong(1);
    (*flog.ts) << "playSong 1" << endl;
}

void MainWindow::pushButton_unshowTraces_clicked()
{
    fleetManager_->ifShowTraces();
    (*flog.ts) << "unshow/show Button pressed" << endl;
}

void MainWindow::velocity_horizontalSlider_sliderMoved(int position)
{
    velocityValue_label_->setText(QString::number(position));
    fleetManager_->setVelocity(position);
}

void MainWindow::mapScale_horizontalSlider_sliderMoved(int position)
{
    mapScaleValue_label_->setText(QString::number(position/10.0));
    map_->setmapScale(position/10.0);
}

void MainWindow::pushButton_Go2POIs_clicked()
{
    fleetManager_->go2Pois();
    (*flog.ts) << "Go2POIs Button pressed." << endl;
}

void MainWindow::pushButton_stopFleet_clicked()
{
    fleetManager_->stopFleet();
}


void MainWindow::action_Cursor_toggled(bool toggleStatus)
{
    if (toggleStatus) {
        map_->setSelectedPaintTool(Util::SelectedPaintTool::CURSOR);
        (*flog.ts) << "Action cursor toggled." << endl;
    }
}

void MainWindow::action_Wall_toggled(bool toggleStatus)
{
    if (toggleStatus) {
        map_->setSelectedPaintTool(Util::SelectedPaintTool::WALL);
        (*flog.ts) << "Action Wall toggled." << endl;
    }
}

void MainWindow::action_Poi_toggled(bool toggleStatus)
{
    if (toggleStatus) {
        map_->setSelectedPaintTool(Util::SelectedPaintTool::POI);
        (*flog.ts) << "Action POI toggled." << endl;
    }
}

void MainWindow::action_Start_toggled(bool toggleStatus)
{
    if (toggleStatus) {
        map_->setSelectedPaintTool(Util::SelectedPaintTool::START);
        (*flog.ts) << "Action Start toggled." << endl;
    }
}

void MainWindow::action_StartVirtual_toggled(bool toggleStatus)
{
    if (toggleStatus) {
        map_->setSelectedPaintTool(Util::SelectedPaintTool::STARTVIRTUAL);
        (*flog.ts) << "Action StartVirtual toggled." << endl;
    }
}

void MainWindow::mapModified()
{
    setWindowModified(true);
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    currentFile_ = fileName;
    setWindowModified(false);

    QString fileNameToShow = currentFile_;
    if(currentFile_.isEmpty())
    {
        fileNameToShow = "untitled.rmap";
    }
    setWindowFilePath(fileNameToShow);
}

void MainWindow::actionSave_triggered()
{
    if(currentFile_.isEmpty())
    {
        actionSaveAs_triggered();
    }
    else
    {
        saveToFile(currentFile_);
    }
}

void MainWindow::actionSaveAs_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as", QDir::homePath(), "ROTFL (*.rmap)");
    if(!fileName.isEmpty()){
        if (!fileName.endsWith(".rmap"))
        {
            fileName.append(".rmap");
        }
        saveToFile(fileName);
    }
}

void MainWindow::actionOpen_triggered()
{
    if (saveUnsavedChanges())
    {
        QString fileName = QFileDialog::getOpenFileName(this,  "Open", QDir::homePath(), "ROTFL (*.rmap)");
        if(!fileName.isEmpty())
        {
            openFile(fileName);
        }
    }
}

bool MainWindow::saveUnsavedChanges()
{
    if(isWindowModified())
    {
        int decision = 0;
        QString fileNameToShow = currentFile_;
        if(currentFile_.isEmpty())
        {
            fileNameToShow = "untitled.rmap";
        }
        QString savingQuestion = "The map \"" + fileNameToShow + "\" has been modified.\nDo you want to save your changes?";
        decision = QMessageBox::warning(this, "ROTFL", savingQuestion, "&Save", "&Discard", "&Cancel", 0, 2);

        if(decision == QMessageBox::AcceptRole)
        {
            actionSave_triggered();
            return true;
        }
        else if(decision == QMessageBox::DestructiveRole)
        {
            return false;
        }
    }
    return true;
}

void MainWindow::saveToFile(QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly)){
        QMessageBox::warning(this, "", tr("No permission for writing to file."));
    }

    QSettings roomba_map(fileName, QSettings::IniFormat);

    unsigned int index = 0;
    roomba_map.beginGroup("Walls");
    std::set<WallQGraphicsLineItem *> walls = fleetManager_->getWalls();
    for (std::set<WallQGraphicsLineItem*>::iterator it = walls.begin();
         it != walls.end(); ++it)
    {
        QLineF line = (*it)->line();
        roomba_map.setValue(QString::number(index),line);
        ++index;
    }
    roomba_map.endGroup();

    roomba_map.beginGroup("POIs");
    std::set<PoiQGraphicsEllipseItem *> pois = fleetManager_->getPOIs();
    index = 0;
    for (std::set<PoiQGraphicsEllipseItem*>::iterator it = pois.begin();
         it != pois.end(); ++it)
    {
        QPointF point = (*it)->pos();
        roomba_map.setValue(QString::number(index),point);
        ++index;
    }
    roomba_map.endGroup();

    setCurrentFile(fileName);
}

void MainWindow::openFile(const QString &fileName)
{
    // Clear the current map first
    fleetManager_->removeAllObjects();

    QSettings roomba_map(fileName, QSettings::IniFormat);
    roomba_map.beginGroup("Walls");
    QStringList walls = roomba_map.childKeys();

    for (QStringList::iterator it = walls.begin(); it != walls.end(); ++it)
    {
        WallQGraphicsLineItem* wall = new WallQGraphicsLineItem((roomba_map.value(*it)).toLineF().x1(),
                                                                (roomba_map.value(*it)).toLineF().y1(),
                                                                (roomba_map.value(*it)).toLineF().x2(),
                                                                (roomba_map.value(*it)).toLineF().y2());
        wall->setFlag(QGraphicsItem::ItemIsSelectable,true);
        wall->setFlag(QGraphicsItem::ItemIsMovable,false); // Disabled so that the mapChanged signal works as expected
        map_->scene()->addItem(wall);
        fleetManager_->addWall(wall);
    }
    roomba_map.endGroup();

    roomba_map.beginGroup("POIs");
    QStringList pois = roomba_map.childKeys();

    for (QStringList::iterator it = pois.begin(); it != pois.end(); ++it)
    {
        PoiQGraphicsEllipseItem* poi = new PoiQGraphicsEllipseItem
                (0.0-Util::POIWIDTH/2.0, 0.0-Util::POIWIDTH/2.0, Util::POIWIDTH, Util::POIWIDTH);
        poi->setPos((roomba_map.value(*it)).toPointF());
        poi->setFlag(QGraphicsItem::ItemIsSelectable,true);
        poi->setFlag(QGraphicsItem::ItemIsMovable,false);  // Disabled so that the mapChanged signal works as expected
        map_->scene()->addItem(poi);
        fleetManager_->addPoi(poi);
    }
    roomba_map.endGroup();

    setCurrentFile(fileName);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(saveUnsavedChanges())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (map_->viewport()->underMouse())
    {
        if (event->type() == QEvent::MouseMove || event->type() == QEvent::DragMove)
        {
            QPoint positionToShow = map_->viewport()->mapFromGlobal(QCursor::pos());
            statusBar()->showMessage("X: " + QString::number(positionToShow.x()*Util::COORDCORRECTION) +
                                     " Y: "+ QString::number(positionToShow.y()*Util::COORDCORRECTION));
        }
        return false;
    }
    else
    {
        statusBar()->clearMessage();
        return QObject::eventFilter(object, event);
    }
}
