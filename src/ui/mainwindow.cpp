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
#include <QTabWidget>
#include <QGroupBox>
#include <QGridLayout>

#include <cmath>
#include <unistd.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qfunctions_wince.h>
#include "threadReader.h"

#include "croi/iRoomba.h"
#include "croi/roombaSerial.h"
#include "croi/roombaRoowifi.h"
#include "croi/roombaVirtual.h"
#include "croi/croiUtil.h"
#include "mapQGraphicsView.h"
#include "fleetManager.h"
#include "flogger.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createMenuBar();
    // TODO: Add better background image for example url(:/widgets/carbon) or color http://www.w3schools.com/cssref/css_colornames.asp
    setStyleSheet("background-color: floralwhite;");

    // Mainwindow could not be smaller than it's contents
    setMinimumSize(Util::PIXELMAPWIDTH+Util::TABWIDTH + 5, Util::PIXELMAPWIDTH + 100);

    fleetManager_ = new FleetManager(this);
    map_ = new MapQGraphicsView(fleetManager_, this);
    map_->setScene(new QGraphicsScene(QRect(0,0,Util::PIXELMAPWIDTH,Util::PIXELMAPWIDTH), this));
    map_->centerOn(0,0);
    fleetManager_->setMap(map_);
    setCentralWidget(map_);
    centralWidget()->setFixedWidth(Util::PIXELMAPWIDTH+2);
    centralWidget()->setFixedHeight(Util::PIXELMAPWIDTH+2);

    tabWidget_ = new QTabWidget(this);
    connect(tabWidget_, SIGNAL(currentChanged(int)), this, SLOT(tabChanged_triggered(int)));  // select the corresponding roomba of the tab
    QDockWidget *dockWidget = new QDockWidget("Control panel", this);
    dockWidget->setWidget(tabWidget_);
    dockWidget->setMinimumSize(Util::TABWIDTH, Util::PIXELMAPWIDTH);
    dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures); // Tab bar is not movable, floatable or closable
    addDockWidget(Qt::RightDockWidgetArea, dockWidget);

    createFleetManagementTab();

    createToolbar();

    setCurrentFile("");

    connect(map_,SIGNAL(mapChanged()),this,SLOT(mapModified()));

    // This event filter is implemented in eventFilter function, keeps mouse coordinates in status bar
    qApp->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete fleetManager_;
}

void MainWindow::createFleetManagementTab()
{
    QVBoxLayout *fleetManagement_layout = new QVBoxLayout;

    // Fleet Management actions groupbox
    QVBoxLayout *fleetActions_layout = new QVBoxLayout;
    fleetManagementEnable_pushButton_ = new QPushButton("Start FleetManagement", this);
    fleetManagementEnable_pushButton_->setDisabled(true);
    fleetManagementEnable_pushButton_->setProperty("Enabled", QVariant(false));
    fleetActions_layout->addWidget(fleetManagementEnable_pushButton_);
    connect(fleetManagementEnable_pushButton_,SIGNAL(clicked()),this,SLOT(pushButton_fleetManagementEnable_clicked()));

    Go2POIs_pushButton_ = new QPushButton("Go 2 POIs", this);
    Go2POIs_pushButton_->setEnabled(false);
    fleetActions_layout->addWidget(Go2POIs_pushButton_);
    connect(Go2POIs_pushButton_,SIGNAL(clicked()),this,SLOT(pushButton_Go2POIs_clicked()));

    clean_pushButton_ = new QPushButton("Clean areas", this);
    clean_pushButton_->setEnabled(false);
    fleetActions_layout->addWidget(clean_pushButton_);
    connect(clean_pushButton_,SIGNAL(clicked()),this,SLOT(pushButton_Clean_clicked()));

    QGroupBox *fleetActions_groupBox = new QGroupBox("Fleet Management actions");
    fleetActions_groupBox->setLayout(fleetActions_layout);
    fleetManagement_layout->addWidget(fleetActions_groupBox);

    // Roomba Common Settings groupbox
    QVBoxLayout *roombaCommonSettings_layout = new QVBoxLayout();
    QLabel *velocity_label = new QLabel("Velocity:");
    roombaCommonSettings_layout->addWidget(velocity_label);
    velocity_horizontalSlider_ = new QSlider(Qt::Horizontal);
    velocity_horizontalSlider_->setMaximum(200);
    velocity_horizontalSlider_->setMinimum(-200);
    connect(velocity_horizontalSlider_,SIGNAL(valueChanged(int)),this,SLOT(velocity_horizontalSlider_sliderMoved(int)));
    roombaCommonSettings_layout->addWidget(velocity_horizontalSlider_);

    QHBoxLayout *velocityValue_layout = new QHBoxLayout;
    velocityValue_label_ = new QLabel("0");
    QLabel *velocityUnit_label = new QLabel("mm/s");
    velocityValue_layout->addWidget(velocityValue_label_);
    velocityValue_layout->addWidget(velocityUnit_label);
    roombaCommonSettings_layout->addLayout(velocityValue_layout);

    QGroupBox *roombaCommonSettings_groupBox = new QGroupBox("Roomba Common Settings");
    roombaCommonSettings_groupBox->setLayout(roombaCommonSettings_layout);
    fleetManagement_layout->addWidget(roombaCommonSettings_groupBox);

    // Map actions groupbox
    QVBoxLayout *map_layout = new QVBoxLayout;
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
    map_layout->addLayout(mapScale_layout);

    QPushButton *tracesDisable_pushButton = new QPushButton("Hide traces", this);
    tracesDisable_pushButton->setProperty("Disabled", QVariant(false));
    map_layout->addWidget(tracesDisable_pushButton);
    connect(tracesDisable_pushButton,SIGNAL(clicked()),this,SLOT(pushButton_tracesDisable_clicked()));

    QGroupBox *mapActions_groupBox = new QGroupBox("Map actions");
    mapActions_groupBox->setLayout(map_layout);
    fleetManagement_layout->addWidget(mapActions_groupBox);

    // Base widget of the tab
    QWidget *fleetManagement_Widget = new QWidget();
    fleetManagement_Widget->setLayout(fleetManagement_layout);
    tabWidget_->addTab(fleetManagement_Widget, "Fleet Management");
}

void MainWindow::addRoombaTab(Croi::IRoomba* roomba)
{
    QVBoxLayout *roombaDashBoard_layout = new QVBoxLayout(); // Layout to fill the whole tab

    QHBoxLayout *status_layout = new QHBoxLayout();  // Layout to show Rooba's Qml-gauges and status information
    QQuickView * qmlView = new QQuickView();
    QWidget *qmlContainer = QWidget::createWindowContainer(qmlView,this);
    qmlContainer->setMinimumSize(400,380);
    qmlContainer->setMaximumSize(400,380);
    qmlContainer->setFocusPolicy(Qt::TabFocus);
    qmlView->setSource(QUrl("qrc:/widgets/gauges"));
    roombaStatuses_.insert(roomba, qmlView->rootObject());
    status_layout->addWidget(qmlContainer);

    QVBoxLayout *statusText_layout = new QVBoxLayout();

    QHBoxLayout *temperature_layout = new QHBoxLayout;
    temperature_labels_.insert(roomba, new QLabel("0"));
    QLabel *temperatureUnit_label = new QLabel("C");
    temperature_layout->addWidget(temperature_labels_.value(roomba));
    temperature_layout->addWidget(temperatureUnit_label);
    statusText_layout->addLayout(temperature_layout);

    QHBoxLayout *chargeLevel_layout = new QHBoxLayout;
    chargeLevel_labels_.insert(roomba, new QLabel("0"));
    QLabel *chargeLevelUnit_label = new QLabel("mAh");
    chargeLevel_layout->addWidget(chargeLevel_labels_.value(roomba));
    chargeLevel_layout->addWidget(chargeLevelUnit_label);
    statusText_layout->addLayout(chargeLevel_layout);

    rmbPosition_labels_.insert(roomba, new QLabel("(0 , 0)"));
    statusText_layout->addWidget(rmbPosition_labels_.value(roomba));

    // Roomba actions groupbox
    QHBoxLayout *action_layout = new QHBoxLayout();
    allMotors_pushButtons_.insert(roomba, new QPushButton("Motors on", this));
    allMotors_pushButtons_.value(roomba)->setIcon(QIcon::fromTheme("system-run"));
    allMotors_pushButtons_.value(roomba)->setProperty("On", QVariant(false));
    allMotors_pushButtons_.value(roomba)->setDisabled(true);
    action_layout->addWidget(allMotors_pushButtons_.value(roomba));
    connect(allMotors_pushButtons_.value(roomba),SIGNAL(clicked()),this,SLOT(pushButton_allMotors_clicked()));
    playSong_pushButtons_.insert(roomba, new QPushButton("&Play song", this));
    playSong_pushButtons_.value(roomba)->setIcon(QIcon::fromTheme("media-playback-start"));
    playSong_pushButtons_.value(roomba)->setDisabled(true);
    action_layout->addWidget(playSong_pushButtons_.value(roomba));
    connect(playSong_pushButtons_.value(roomba),SIGNAL(clicked()),this,SLOT(pushButton_playSong_clicked()));

    QGroupBox *action_groupBox = new QGroupBox("Roomba controls");
    action_groupBox->setLayout(action_layout);
    statusText_layout->addWidget(action_groupBox);

    status_layout->addLayout(statusText_layout);

    roombaDashBoard_layout->addLayout(status_layout);

    // Roomba settings groupbox
    QVBoxLayout *connect_layout = new QVBoxLayout();
    QHBoxLayout *ipLineEdit_layout = new QHBoxLayout();
    QGroupBox *connect_groupBox = new QGroupBox();
    roombaNameLineEdits_.insert(roomba, new QLineEdit(""));
    connection_pushButtons_.insert(roomba, new QPushButton("", this));
    connect(connection_pushButtons_.value(roomba),SIGNAL(clicked()),this,SLOT(pushButton_Connection_clicked()));
    if (qobject_cast<Croi::RoombaRoowifi*>(roomba))
    {
        ip1LineEdits_.insert(roomba, new QLineEdit("192"));
        ip2LineEdits_.insert(roomba, new QLineEdit("168"));
        ip3LineEdits_.insert(roomba, new QLineEdit("1"));
        ip4LineEdits_.insert(roomba, new QLineEdit("145"));
        ipLineEdit_layout->addWidget(roombaNameLineEdits_.value(roomba));
        ipLineEdit_layout->addWidget(ip1LineEdits_.value(roomba));
        ipLineEdit_layout->addWidget(ip2LineEdits_.value(roomba));
        ipLineEdit_layout->addWidget(ip3LineEdits_.value(roomba));
        ipLineEdit_layout->addWidget(ip4LineEdits_.value(roomba));
        roombaNameLineEdits_.value(roomba)->setText(QString("Roomba %1").arg(tabWidget_->count()));
        connection_pushButtons_.value(roomba)->setText("Connect");
        connection_pushButtons_.value(roomba)->setProperty("Connected", QVariant("ROOWIFI-DISCONNECTED"));
        connection_pushButtons_.value(roomba)->setIcon(QIcon::fromTheme("network-wireless"));
        connect_groupBox->setTitle("Virtual Roomba settings");
    }
    else if (qobject_cast<Croi::RoombaVirtual*>(roomba))
    {
        ipLineEdit_layout->addWidget(roombaNameLineEdits_.value(roomba));
        connection_pushButtons_.value(roomba)->setText("Rename");
        roombaNameLineEdits_.value(roomba)->setText(QString("Virtual Roomba %1").arg(tabWidget_->count()));
        connect_groupBox->setTitle("Roowifi settings");
    }
    ipLineEdit_layout->addWidget(connection_pushButtons_.value(roomba));
    connect_layout->addLayout(ipLineEdit_layout);
    connect_groupBox->setLayout(connect_layout);
    roombaDashBoard_layout->addWidget(connect_groupBox);

    // Layout to moving roomba's location on map
    QHBoxLayout *moving_layout = new QHBoxLayout();
    // Manual driving layout
    QGridLayout *manualDriving_layout = new QGridLayout();
    turnCcw_pushButtons_.insert(roomba, new QPushButton());
    turnCcw_pushButtons_.value(roomba)->setIcon(QIcon(":/icons/rotate_left"));
    turnCcw_pushButtons_.value(roomba)->setCheckable(true);
    turnCcw_pushButtons_.value(roomba)->setDisabled(true);
    turnCcw_pushButtons_.value(roomba)->setToolTip("Turn left");
    manualDriving_layout->addWidget(turnCcw_pushButtons_.value(roomba), 1, 1);
    connect(turnCcw_pushButtons_.value(roomba),SIGNAL(clicked()),this,SLOT(pushButton_turnCcw_clicked()));

    driveForward_toolButtons_.insert(roomba, new QToolButton());
    driveForward_toolButtons_.value(roomba)->setArrowType(Qt::UpArrow);
    driveForward_toolButtons_.value(roomba)->setCheckable(true);
    driveForward_toolButtons_.value(roomba)->setDisabled(true);
    driveForward_toolButtons_.value(roomba)->setToolTip("Drive forward");
    manualDriving_layout->addWidget(driveForward_toolButtons_.value(roomba), 1, 2);
    connect(driveForward_toolButtons_.value(roomba),SIGNAL(clicked()),this,SLOT(toolButton_driveForward_clicked()));

    turnCw_pushButtons_.insert(roomba, new QPushButton());
    turnCw_pushButtons_.value(roomba)->setIcon(QIcon(":/icons/rotate_right"));
    turnCw_pushButtons_.value(roomba)->setCheckable(true);
    turnCw_pushButtons_.value(roomba)->setDisabled(true);
    turnCw_pushButtons_.value(roomba)->setToolTip("Turn right");
    manualDriving_layout->addWidget(turnCw_pushButtons_.value(roomba), 1, 3);
    connect(turnCw_pushButtons_.value(roomba),SIGNAL(clicked()),this,SLOT(pushButton_turnCw_clicked()));

    driveBackward_toolButtons_.insert(roomba, new QToolButton());
    driveBackward_toolButtons_.value(roomba)->setArrowType(Qt::DownArrow);
    driveBackward_toolButtons_.value(roomba)->setCheckable(true);
    driveBackward_toolButtons_.value(roomba)->setDisabled(true);
    driveBackward_toolButtons_.value(roomba)->setToolTip("Drive backward");
    manualDriving_layout->addWidget(driveBackward_toolButtons_.value(roomba), 2, 2);
    connect(driveBackward_toolButtons_.value(roomba),SIGNAL(clicked()),this,SLOT(toolButton_driveBackward_clicked()));

    QGroupBox *manualDriving_groupBox = new QGroupBox("Manual driving");
    manualDriving_groupBox->setLayout(manualDriving_layout);
    // Stylesheet adjusts the size of all buttons in groupbox
    manualDriving_groupBox->setStyleSheet("QPushButton, QToolButton {width: 16px; height: 16px}");
    moving_layout->addWidget(manualDriving_groupBox);

    // Manual correction layout
    QGridLayout *manualCorrection_layout = new QGridLayout();
    correctCcw_pushButtons_.insert(roomba, new QPushButton());
    correctCcw_pushButtons_.value(roomba)->setIcon(QIcon(":/icons/rotate_left"));
    correctCcw_pushButtons_.value(roomba)->setToolTip("Correct angle counterclockwise by 2 degrees");
    manualCorrection_layout->addWidget(correctCcw_pushButtons_.value(roomba), 1, 1);
    connect(correctCcw_pushButtons_.value(roomba),SIGNAL(clicked()),this,SLOT(pushButton_correctCcw_clicked()));

    correctUp_toolButtons_.insert(roomba, new QToolButton());
    correctUp_toolButtons_.value(roomba)->setArrowType(Qt::UpArrow);
    correctUp_toolButtons_.value(roomba)->setToolTip("Correct location to up by 2 cm");
    manualCorrection_layout->addWidget(correctUp_toolButtons_.value(roomba), 1, 2);
    connect(correctUp_toolButtons_.value(roomba),SIGNAL(clicked()),this,SLOT(toolButton_correctUp_clicked()));

    correctCw_pushButtons_.insert(roomba, new QPushButton());
    correctCw_pushButtons_.value(roomba)->setIcon(QIcon(":/icons/rotate_right"));
    correctCw_pushButtons_.value(roomba)->setToolTip("Correct angle clockwise by 2 degrees");
    manualCorrection_layout->addWidget(correctCw_pushButtons_.value(roomba), 1, 3);
    connect(correctCw_pushButtons_.value(roomba),SIGNAL(clicked()),this,SLOT(pushButton_correctCw_clicked()));

    correctLeft_toolButtons_.insert(roomba, new QToolButton());
    correctLeft_toolButtons_.value(roomba)->setArrowType(Qt::LeftArrow);
    correctLeft_toolButtons_.value(roomba)->setToolTip("Correct location to left by 2 cm");
    manualCorrection_layout->addWidget(correctLeft_toolButtons_.value(roomba), 2, 1);
    connect(correctLeft_toolButtons_.value(roomba),SIGNAL(clicked()),this,SLOT(toolButton_correctLeft_clicked()));

    correctDown_toolButtons_.insert(roomba, new QToolButton());
    correctDown_toolButtons_.value(roomba)->setArrowType(Qt::DownArrow);
    correctDown_toolButtons_.value(roomba)->setToolTip("Correct location to down by 2 cm");
    manualCorrection_layout->addWidget(correctDown_toolButtons_.value(roomba), 2, 2);
    connect(correctDown_toolButtons_.value(roomba),SIGNAL(clicked()),this,SLOT(toolButton_correctDown_clicked()));

    correctRight_toolButtons_.insert(roomba, new QToolButton());
    correctRight_toolButtons_.value(roomba)->setArrowType(Qt::RightArrow);
    correctRight_toolButtons_.value(roomba)->setToolTip("Correct location to right by 2 cm");
    manualCorrection_layout->addWidget(correctRight_toolButtons_.value(roomba), 2, 3);
    connect(correctRight_toolButtons_.value(roomba),SIGNAL(clicked()),this,SLOT(toolButton_correctRight_clicked()));

    resetAngle_pushButtons_.insert(roomba, new QPushButton());
    resetAngle_pushButtons_.value(roomba)->setIcon(QIcon(":/icons/reset_rotation"));
    resetAngle_pushButtons_.value(roomba)->setToolTip("Reset angle");
    manualCorrection_layout->addWidget(resetAngle_pushButtons_.value(roomba), 1, 4);
    connect(resetAngle_pushButtons_.value(roomba),SIGNAL(clicked()),this,SLOT(pushButton_resetAngle_clicked()));

    QGroupBox *manualCorrection_groupBox = new QGroupBox("Manual location correction");
    manualCorrection_groupBox->setLayout(manualCorrection_layout);
    // Stylesheet adjusts the size of all buttons in groupbox
    manualCorrection_groupBox->setStyleSheet("QPushButton, QToolButton {width: 16px; height: 16px}");
    moving_layout->addWidget(manualCorrection_groupBox);

    roombaDashBoard_layout->addLayout(moving_layout);

    // Base widget of the tab
    QWidget *roombaDashBoard_widget = new QWidget(this);
    roombaDashBoard_widget->setLayout(roombaDashBoard_layout);
    roomba_Widgets_.append(roombaDashBoard_widget);
    int currentIndex = tabWidget_->addTab(roombaDashBoard_widget, roombaNameLineEdits_.value(roomba)->text());
    roombaTabs_.insert(currentIndex, roomba);
    tabWidget_->setCurrentIndex(currentIndex);
    // Virtual Roomba is always connected
    if (qobject_cast<Croi::RoombaVirtual*>(roomba))
    {
        tabWidget_->setTabIcon(currentIndex, QIcon::fromTheme("network-wireless"));
    }
    else if (qobject_cast<Croi::RoombaRoowifi*>(roomba))
    {
        tabWidget_->setTabIcon(currentIndex, QIcon::fromTheme("network-offline"));
        // Roombas location couldn't be corrected before it's connected
        resetAngle_pushButtons_.value(roomba)->setDisabled(true);
        correctLeft_toolButtons_.value(roomba)->setDisabled(true);
        correctRight_toolButtons_.value(roomba)->setDisabled(true);
        correctUp_toolButtons_.value(roomba)->setDisabled(true);
        correctDown_toolButtons_.value(roomba)->setDisabled(true);
        correctCw_pushButtons_.value(roomba)->setDisabled(true);
        correctCcw_pushButtons_.value(roomba)->setDisabled(true);
    }
    cursor_action_->trigger();  // Change to Cursor after adding a Roomba to prevent accidentially adding too many
    fleetManagementEnable_pushButton_->setEnabled(true); // Enable starting fleet management when adding first roomba
}

void MainWindow::setRoombaStatusData(Croi::IRoomba* selectedRoomba)
{
    // TODO: Remove double bookkeeping of selected roomba
    if (selectedRoomba == selectedRoomba_)
    {
        temperature_labels_.value(selectedRoomba)->setText( QString::number( ( unsigned char )( selectedRoomba->getTemperature() ) ) );
        chargeLevel_labels_.value(selectedRoomba)->setText( QString::number( (unsigned short)( selectedRoomba->getChargeLevel() ) ) );
        QPointF rmbPosition = selectedRoomba->getLoc();
        rmbPosition_labels_.value(selectedRoomba)->setText( "(" + QString::number(rmbPosition.x()*Util::COORDCORRECTION, 'f', 0) +
                                                            " , " + QString::number(rmbPosition.y()*Util::COORDCORRECTION, 'f', 0) + ")" );

        //QML
        QVariant returnedValue;
        QMetaObject::invokeMethod(roombaStatuses_.value(selectedRoomba), "setBatteryLevelmAh", Q_RETURN_ARG(QVariant, returnedValue),
                                  Q_ARG(QVariant, selectedRoomba->getChargeLevel()), Q_ARG(QVariant, selectedRoomba->getBatteryLevel()) );

        QMetaObject::invokeMethod(roombaStatuses_.value(selectedRoomba), "setSpeed", Q_RETURN_ARG(QVariant, returnedValue),
                                  Q_ARG(QVariant, abs(selectedRoomba->getVelocity())));

        QMetaObject::invokeMethod(roombaStatuses_.value(selectedRoomba), "setDirection", Q_RETURN_ARG(QVariant, returnedValue),
                                  Q_ARG(QVariant, abs(selectedRoomba->getCurrentAngle()*180/Util::PI)));

        QMetaObject::invokeMethod(roombaStatuses_.value(selectedRoomba), "setTemperature", Q_RETURN_ARG(QVariant, returnedValue),
                                  Q_ARG(QVariant, abs(selectedRoomba->getTemperature())));

        QMetaObject::invokeMethod(roombaStatuses_.value(selectedRoomba), "setDistance", Q_RETURN_ARG(QVariant, returnedValue),
                                  Q_ARG(QVariant, QString::number(selectedRoomba->getTotalDistance() * Util::COORDCORRECTION, 'f', 0)));
    }
}

void MainWindow::createToolbar()
{
    ui->mainToolBar->setMovable(false);
    ui->mainToolBar->setFloatable(false);
    ui->mainToolBar->setWindowTitle("Toolbar");

    QActionGroup *actionGroup = new QActionGroup(this);

    cursor_action_ = new QAction("Select", actionGroup);
    cursor_action_->setIcon(QIcon(":/icons/clear_hand"));
    cursor_action_->setCheckable(true);
    cursor_action_->setChecked(true);
    map_->setSelectedPaintTool(Util::SelectedPaintTool::CURSOR);
    connect(cursor_action_,SIGNAL(toggled(bool)),this,SLOT(action_Cursor_toggled(bool)));
    ui->mainToolBar->addAction(cursor_action_);

    wall_action_ = new QAction("Add Wall", actionGroup);
    wall_action_->setIcon(QIcon(":icons/graphics/wall"));
    wall_action_->setCheckable(true);
    connect(wall_action_,SIGNAL(toggled(bool)),this,SLOT(action_Wall_toggled(bool)));
    ui->mainToolBar->addAction(wall_action_);

    QAction* poi_action = new QAction("Add Point of Interest", actionGroup);
    poi_action->setIcon(QIcon(":/icons/poi"));
    poi_action->setCheckable(true);
    connect(poi_action,SIGNAL(toggled(bool)),this,SLOT(action_Poi_toggled(bool)));
    ui->mainToolBar->addAction(poi_action);

    QAction* atc_action = new QAction("Add Area to Clean", actionGroup);
    atc_action->setIcon(QIcon(":/icons/graphics/atc"));
    atc_action->setCheckable(true);
    connect(atc_action,SIGNAL(toggled(bool)),this,SLOT(action_ATC_toggled(bool)));
    ui->mainToolBar->addAction(atc_action);

    start_action_ = new QAction("Add Roomba", actionGroup);
    start_action_->setIcon(QIcon(":/icons/roomba_small"));
    start_action_->setCheckable(true);
    connect(start_action_,SIGNAL(toggled(bool)),this,SLOT(action_Start_toggled(bool)));
    ui->mainToolBar->addAction(start_action_);

    startVirtual_action_ = new QAction("Add virtual Roomba", actionGroup);
    startVirtual_action_->setIcon(QIcon(":/icons/roomba_virtual_small"));
    startVirtual_action_->setCheckable(true);
    connect(startVirtual_action_,SIGNAL(toggled(bool)),this,SLOT(action_StartVirtual_toggled(bool)));
    ui->mainToolBar->addAction(startVirtual_action_);

    ui->mainToolBar->adjustSize();
}

void MainWindow::handleUIElementsChangeAllTabsState(bool state)
{
    int currentIndex = tabWidget_->currentIndex();
    bool enabled = !state;
    for (int tab = 0; tab < tabWidget_->count(); ++tab)
    {
        tabWidget_->setTabEnabled(tab, enabled);
    }
    tabWidget_->setCurrentIndex(currentIndex);
}

void MainWindow::pushButton_Connection_clicked()
{
    tabWidget_->setTabText(tabWidget_->currentIndex(), roombaNameLineEdits_.value(selectedRoomba_)->text());
    // "Connected" property is only set for RoombaRoowifi
    if (connection_pushButtons_.value(selectedRoomba_)->property("Connected") == QVariant("ROOWIFI-DISCONNECTED"))
    {
        QString ip = ip1LineEdits_.value(selectedRoomba_)->text() + "." + ip2LineEdits_.value(selectedRoomba_)->text()
                + "." + ip3LineEdits_.value(selectedRoomba_)->text() + "." + ip4LineEdits_.value(selectedRoomba_)->text();
        std::string stdip = ip.toStdString();
        connection_pushButtons_.value(selectedRoomba_)->setText("Connecting...");
        connection_pushButtons_.value(selectedRoomba_)->setDisabled(true);
        handleUIElementsConnectionStateChange(true);
        handleUIElementsChangeAllTabsState(true);
        selectedRoomba_->rmb_connect(stdip);
        (*flog.ts) << "Connect Button pressed." << endl;
    }

    else if(connection_pushButtons_.value(selectedRoomba_)->property("Connected") == QVariant("ROOWIFI-CONNECTED"))
    {
        connection_pushButtons_.value(selectedRoomba_)->setProperty("Connected", QVariant("ROOWIFI-DISCONNECTED"));
        connection_pushButtons_.value(selectedRoomba_)->setText("Connect");
        connection_pushButtons_.value(selectedRoomba_)->setIcon(QIcon::fromTheme("network-wireless"));
        selectedRoomba_->disconnect();
        resetRoombaStatusInfo();
        handleUIElementsConnectionStateChange(false);
        tabWidget_->setTabIcon(tabWidget_->currentIndex(), QIcon::fromTheme("network-offline"));
        (*flog.ts) << "Disconnect Button pressed." << endl;
    }
}

void MainWindow::connectionEstablished()
{
    connection_pushButtons_.value(selectedRoomba_)->setProperty("Connected", QVariant("ROOWIFI-CONNECTED"));
    connection_pushButtons_.value(selectedRoomba_)->setText("Disconnect");
    connection_pushButtons_.value(selectedRoomba_)->setEnabled(true);
    connection_pushButtons_.value(selectedRoomba_)->setIcon(QIcon::fromTheme("network-offline"));
    tabWidget_->setTabIcon(tabWidget_->currentIndex(), QIcon::fromTheme("network-wireless"));
    allMotors_pushButtons_.value(selectedRoomba_)->setEnabled(true);
    playSong_pushButtons_.value(selectedRoomba_)->setEnabled(true);
    driveForward_toolButtons_.value(selectedRoomba_)->setEnabled(true);
    driveBackward_toolButtons_.value(selectedRoomba_)->setEnabled(true);
    turnCcw_pushButtons_.value(selectedRoomba_)->setEnabled(true);
    turnCw_pushButtons_.value(selectedRoomba_)->setEnabled(true);
    resetAngle_pushButtons_.value(selectedRoomba_)->setEnabled(true);
    correctLeft_toolButtons_.value(selectedRoomba_)->setEnabled(true);
    correctRight_toolButtons_.value(selectedRoomba_)->setEnabled(true);
    correctUp_toolButtons_.value(selectedRoomba_)->setEnabled(true);
    correctDown_toolButtons_.value(selectedRoomba_)->setEnabled(true);
    correctCw_pushButtons_.value(selectedRoomba_)->setEnabled(true);
    correctCcw_pushButtons_.value(selectedRoomba_)->setEnabled(true);

    handleUIElementsChangeAllTabsState(false);
    (*flog.ts) << "Connection established." << endl;
}

void MainWindow::connectionFailed()
{
    connection_pushButtons_.value(selectedRoomba_)->setText("Connect");
    connection_pushButtons_.value(selectedRoomba_)->setEnabled(true);
    handleUIElementsConnectionStateChange(false);
    handleUIElementsChangeAllTabsState(false);
    (*flog.ts) << "Connection failed." << endl;
}

void MainWindow::handleUIElementsConnectionStateChange(bool state)
{
    ip1LineEdits_.value(selectedRoomba_)->setDisabled(state);
    ip2LineEdits_.value(selectedRoomba_)->setDisabled(state);
    ip3LineEdits_.value(selectedRoomba_)->setDisabled(state);
    ip4LineEdits_.value(selectedRoomba_)->setDisabled(state);
    roombaNameLineEdits_.value(selectedRoomba_)->setDisabled(state);
    driveForward_toolButtons_.value(selectedRoomba_)->setEnabled(state);
    driveBackward_toolButtons_.value(selectedRoomba_)->setEnabled(state);
    turnCw_pushButtons_.value(selectedRoomba_)->setEnabled(state);
    turnCcw_pushButtons_.value(selectedRoomba_)->setEnabled(state);
    start_action_->setDisabled(state);
    startVirtual_action_->setDisabled(state);
}

void MainWindow::handleUIElementsControlModeStateChange(bool state)
{
    for (auto roomba : roombaTabs_.values())
    {
        if (qobject_cast<Croi::RoombaRoowifi*>(roomba))
        {
            ip1LineEdits_.value(roomba)->setDisabled(state);
            ip2LineEdits_.value(roomba)->setDisabled(state);
            ip3LineEdits_.value(roomba)->setDisabled(state);
            ip4LineEdits_.value(roomba)->setDisabled(state);
        }
        roombaNameLineEdits_.value(roomba)->setDisabled(state);
        connection_pushButtons_.value(roomba)->setDisabled(state);
        allMotors_pushButtons_.value(roomba)->setDisabled(state);
        playSong_pushButtons_.value(roomba)->setDisabled(state);
        resetAngle_pushButtons_.value(roomba)->setDisabled(state);
        correctLeft_toolButtons_.value(roomba)->setDisabled(state);
        correctRight_toolButtons_.value(roomba)->setDisabled(state);
        correctUp_toolButtons_.value(roomba)->setDisabled(state);
        correctDown_toolButtons_.value(roomba)->setDisabled(state);
        correctCw_pushButtons_.value(roomba)->setDisabled(state);
        correctCcw_pushButtons_.value(roomba)->setDisabled(state);
        driveForward_toolButtons_.value(roomba)->setDisabled(state);
        driveBackward_toolButtons_.value(roomba)->setDisabled(state);
        turnCw_pushButtons_.value(roomba)->setDisabled(state);
        turnCcw_pushButtons_.value(roomba)->setDisabled(state);
    }
    clean_pushButton_->setEnabled(state);
    Go2POIs_pushButton_->setEnabled(state);
    wall_action_->setDisabled(state);
    start_action_->setDisabled(state);
    startVirtual_action_->setDisabled(state);
}

void MainWindow::pushButton_Clean_clicked()
{
    fleetManager_->clean();
    (*flog.ts) << "Clean Button pressed." << endl;
}

void MainWindow::pushButton_allMotors_clicked()
{
    if (allMotors_pushButtons_.value(selectedRoomba_)->property("On") == QVariant(false))
    {
        allMotors_pushButtons_.value(selectedRoomba_)->setProperty("On", QVariant(true));
        allMotors_pushButtons_.value(selectedRoomba_)->setText("Motors off");
        fleetManager_->allMotorsOn();
        (*flog.ts) << "Motors on Button pressed." << endl;
    }

    else
    {
        allMotors_pushButtons_.value(selectedRoomba_)->setProperty("On", QVariant(false));
        allMotors_pushButtons_.value(selectedRoomba_)->setText("Motors on");
        fleetManager_->allMotorsOff();
        (*flog.ts) << "Motors off Button pressed." << endl;
    }

}

void MainWindow::pushButton_resetAngle_clicked()
{
    fleetManager_->resetAngle();
    (*flog.ts) << "ResetAngle Button pressed." << endl;
}

void MainWindow::toolButton_correctLeft_clicked()
{
    fleetManager_->correctLocation(Util::Direction::W);
}

void MainWindow::toolButton_correctRight_clicked()
{
    fleetManager_->correctLocation(Util::Direction::E);
}

void MainWindow::toolButton_correctUp_clicked()
{
    fleetManager_->correctLocation(Util::Direction::N);
}

void MainWindow::toolButton_correctDown_clicked()
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

void MainWindow::handleUIElementsDrivingStateChange()
{
    driveForward_toolButtons_.value(selectedRoomba_)->setChecked(false);
    driveBackward_toolButtons_.value(selectedRoomba_)->setChecked(false);
    turnCcw_pushButtons_.value(selectedRoomba_)->setChecked(false);
    turnCw_pushButtons_.value(selectedRoomba_)->setChecked(false);
}

void MainWindow::toolButton_driveForward_clicked()
{
    if (velocity_horizontalSlider_->value() < 0)
    {
        velocity_horizontalSlider_->setValue(velocity_horizontalSlider_->value()*-1);
    }
    else if (velocity_horizontalSlider_->value() == 0)
    {
        velocity_horizontalSlider_->setValue(100);
    }
    if (driveForward_toolButtons_.value(selectedRoomba_)->isChecked())
    {
        handleUIElementsDrivingStateChange();
        driveForward_toolButtons_.value(selectedRoomba_)->setChecked(true);
        fleetManager_->drive(velocity_horizontalSlider_->value(), Util::RADSTRAIGHT);
        (*flog.ts) << "UpArrow" << endl;
    }
    else
    {
        driveForward_toolButtons_.value(selectedRoomba_)->setChecked(false);
        fleetManager_->drive(0, Util::RADSTRAIGHT);
        (*flog.ts) << "Stop" << endl;
    }
}

void MainWindow::toolButton_driveBackward_clicked()
{
    if (velocity_horizontalSlider_->value() > 0)
    {
        velocity_horizontalSlider_->setValue(velocity_horizontalSlider_->value()*-1);
    }
    else if (velocity_horizontalSlider_->value() == 0)
    {
        velocity_horizontalSlider_->setValue(-100);
    }
    if (driveBackward_toolButtons_.value(selectedRoomba_)->isChecked())
    {
        handleUIElementsDrivingStateChange();
        driveBackward_toolButtons_.value(selectedRoomba_)->setChecked(true);
        fleetManager_->drive(velocity_horizontalSlider_->value(), Util::RADSTRAIGHT);
        (*flog.ts) << "BackArrow" << endl;
    }
    else
    {
        driveBackward_toolButtons_.value(selectedRoomba_)->setChecked(false);
        fleetManager_->drive(0, Util::RADSTRAIGHT);
        (*flog.ts) << "Stop" << endl;
    }
}

void MainWindow::pushButton_turnCw_clicked()
{
    if (velocity_horizontalSlider_->value() < 0)
    {
        velocity_horizontalSlider_->setValue(velocity_horizontalSlider_->value()*-1);
    }
    else if (velocity_horizontalSlider_->value() == 0)
    {
        velocity_horizontalSlider_->setValue(100);
    }
    if (turnCw_pushButtons_.value(selectedRoomba_)->isChecked())
    {
        handleUIElementsDrivingStateChange();
        turnCw_pushButtons_.value(selectedRoomba_)->setChecked(true);
        fleetManager_->drive(velocity_horizontalSlider_->value(), Util::RADTURNCW);
        (*flog.ts) << "Turn clockwise" << endl;
    }
    else
    {
        turnCw_pushButtons_.value(selectedRoomba_)->setChecked(false);
        fleetManager_->drive(0, Util::RADSTRAIGHT);
        (*flog.ts) << "Stop" << endl;
    }
}

void MainWindow::pushButton_turnCcw_clicked()
{
    if (velocity_horizontalSlider_->value() < 0)
    {
        velocity_horizontalSlider_->setValue(velocity_horizontalSlider_->value()*-1);
    }
    else if (velocity_horizontalSlider_->value() == 0)
    {
        velocity_horizontalSlider_->setValue(100);
    }
    if (turnCcw_pushButtons_.value(selectedRoomba_)->isChecked())
    {
        handleUIElementsDrivingStateChange();
        turnCcw_pushButtons_.value(selectedRoomba_)->setChecked(true);
        fleetManager_->drive(velocity_horizontalSlider_->value(), Util::RADTURNCCW);
        (*flog.ts) << "Turn counterclockwise" << endl;
    }
    else
    {
        turnCcw_pushButtons_.value(selectedRoomba_)->setChecked(false);
        fleetManager_->drive(0, Util::RADSTRAIGHT);
        (*flog.ts) << "Stop" << endl;
    }
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

void MainWindow::pushButton_tracesDisable_clicked()
{
    QPushButton *sender = qobject_cast<QPushButton*>(QObject::sender());
    if (sender->property("Disabled") == QVariant(false))
    {
        sender->setProperty("Disabled", QVariant(true));
        sender->setText("Show traces");
        (*flog.ts) << "Hide traces button pressed" << endl;
    }
    else
    {
        sender->setProperty("Disabled", QVariant(false));
        sender->setText("Hide traces");
        (*flog.ts) << "Show traces button pressed" << endl;
    }
    fleetManager_->ifShowTraces();
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

void MainWindow::pushButton_fleetManagementEnable_clicked()
{
    QPushButton *sender = qobject_cast<QPushButton*>(QObject::sender());
    if (sender->property("Enabled")  == QVariant(false))
    {
        map_->setContextMenuPolicy(Qt::NoContextMenu); // Disable removing of items from context menu
        sender->setProperty("Enabled", QVariant(true));
        sender->setText("Stop Fleet Management");
        // Set selected paint tool to cursor, so adding new roombas or walls is not possible
        cursor_action_->trigger();
        handleUIElementsControlModeStateChange(true);
    }
    else
    {
        map_->setContextMenuPolicy(Qt::DefaultContextMenu); // Enable removing of items from context menu
        sender->setProperty("Enabled", QVariant(false));
        sender->setText("Start Fleet Management");
        handleUIElementsControlModeStateChange(false);
        fleetManager_->stopFleet(false);
    }
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

void MainWindow::action_ATC_toggled(bool toggleStatus)
{
    if (toggleStatus) {
        map_->setSelectedPaintTool(Util::SelectedPaintTool::ATC);
        (*flog.ts) << "Action ATC toggled." << endl;
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
    QVector<PoiQGraphicsEllipseItem *> pois = fleetManager_->getPOIs();
    index = 0;
    for (QVector<PoiQGraphicsEllipseItem*>::iterator it = pois.begin();
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
        WallQGraphicsLineItem* wall = new WallQGraphicsLineItem(fleetManager_,
                                                                (roomba_map.value(*it)).toLineF().x1(),
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
                (fleetManager_, 0.0-Util::POIWIDTH/2.0, 0.0-Util::POIWIDTH/2.0, Util::POIWIDTH, Util::POIWIDTH);
        poi->setPos((roomba_map.value(*it)).toPointF());
        poi->setFlag(QGraphicsItem::ItemIsSelectable,true);
        poi->setFlag(QGraphicsItem::ItemIsMovable,false);  // Disabled so that the mapChanged signal works as expected
        map_->scene()->addItem(poi);
        fleetManager_->addPoi(poi);
    }
    roomba_map.endGroup();

    setCurrentFile(fileName);
}

void MainWindow::createMenuBar()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));

    QAction* openAct = new QAction(tr("&Open"),this);
    openAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    openAct->setIcon(QIcon::fromTheme("document-open"));
    connect(openAct, SIGNAL(triggered()),this,SLOT(actionOpen_triggered()));
    fileMenu->addAction(openAct);

    QAction* saveAct = new QAction(tr("&Save"),this);
    saveAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    saveAct->setIcon(QIcon::fromTheme("document-save"));
    connect(saveAct, SIGNAL(triggered()),this,SLOT(actionSave_triggered()));
    fileMenu->addAction(saveAct);

    QAction* saveAsAct = new QAction(tr("S&ave as"),this);
    saveAsAct->setShortcut(QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_S));
    saveAsAct->setIcon(QIcon::fromTheme("document-save-as"));
    connect(saveAsAct, SIGNAL(triggered()),this,SLOT(actionSaveAs_triggered()));
    fileMenu->addAction(saveAsAct);

    QAction* quitAct = new QAction(tr("&Quit"),this);
    quitAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    quitAct->setIcon(QIcon::fromTheme("application-exit"));
    connect(quitAct, SIGNAL(triggered()),this,SLOT(close()));
    fileMenu->addAction(quitAct);

    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction* aboutQtAct = new QAction(tr("About &Qt"),this);
    connect(aboutQtAct, SIGNAL(triggered()),this,SLOT(action_AboutQt_triggered()));
    helpMenu->addAction(aboutQtAct);

    QAction* aboutAct = new QAction(tr("&About"),this);
    aboutAct->setIcon(QIcon::fromTheme("help-about"));
    connect(aboutAct, SIGNAL(triggered()),this,SLOT(action_About_triggered()));
    helpMenu->addAction(aboutAct);
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
            //the QCursor::pos() has to be mapped to the map_ and then to the scene since we are interested
            //about the scene coordinates, not the possibly scaled view of it
            QPoint positionToShow = map_->mapToScene(map_->viewport()->mapFromGlobal(QCursor::pos())).toPoint();
            //Util::COORDCORRECTION has to be used when wanting to output real world coordinates
            statusBar()->showMessage("X: " + QString::number(positionToShow.x()*Util::COORDCORRECTION, 'f', 0) +
                                     " Y: "+ QString::number(positionToShow.y()*Util::COORDCORRECTION, 'f', 0));
        }
        return false;
    }
    else
    {
        statusBar()->clearMessage();
        return QObject::eventFilter(object, event);
    }
}

void MainWindow::setSelectedRoombaTab(Croi::IRoomba * roomba)
{
    stopAllManuallyControlledRoombas();
    int selectedTab = roombaTabs_.key(roomba, -1);
    if (selectedTab != -1)
    {
        qDebug() << "setSelectedTab: "<< selectedTab << "\n";
        selectedRoomba_ = roomba;
        tabWidget_->setCurrentIndex(selectedTab);
    }
}

void MainWindow::tabChanged_triggered(int index)
{
    stopAllManuallyControlledRoombas();
    map_->scene()->clearSelection(); // Clear selection even when changing to Fleet Management tab
    Croi::IRoomba * selectedRoomba = roombaTabs_.value(index, NULL);
    if (selectedRoomba != NULL)
    {
        qDebug() << "tabChanged_triggered" << index << "\n";
        selectedRoomba_ = selectedRoomba;
        selectedRoomba->getIcon()->setSelected(true);
    }
}

void MainWindow::stopAllManuallyControlledRoombas()
{
    for (auto roomba : roombaTabs_.values())
    {
        roomba->drive(0, Util::RADSTRAIGHT);
        driveForward_toolButtons_.value(roomba)->setChecked(false);
        driveBackward_toolButtons_.value(roomba)->setChecked(false);
        turnCcw_pushButtons_.value(roomba)->setChecked(false);
        turnCw_pushButtons_.value(roomba)->setChecked(false);
    }
}

void MainWindow::resetRoombaStatusInfo()
{
    temperature_labels_.value(selectedRoomba_)->setText( QString::number( ( unsigned char )( 0 ) ) );
    chargeLevel_labels_.value(selectedRoomba_)->setText( QString::number( (unsigned short)( 0 ) ) );
    QPointF rmbPosition = selectedRoomba_->getLoc();
    rmbPosition_labels_.value(selectedRoomba_)->setText( "(" + QString::number(rmbPosition.x()*Util::COORDCORRECTION, 'f', 0) +
                                                        " , " + QString::number(rmbPosition.y()*Util::COORDCORRECTION, 'f', 0) + ")" );

    //QML
    QVariant returnedValue;
    QMetaObject::invokeMethod(roombaStatuses_.value(selectedRoomba_), "setBatteryLevelmAh", Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, 0) );

    QMetaObject::invokeMethod(roombaStatuses_.value(selectedRoomba_), "setSpeed", Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, 0));

    QMetaObject::invokeMethod(roombaStatuses_.value(selectedRoomba_), "setDirection", Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, abs(selectedRoomba_->getCurrentAngle()*180/Util::PI)));

    QMetaObject::invokeMethod(roombaStatuses_.value(selectedRoomba_), "setTemperature", Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, 0));

    QMetaObject::invokeMethod(roombaStatuses_.value(selectedRoomba_), "setDistance", Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, 0));
}

void MainWindow::action_About_triggered()
{
    const QString aboutDescription =
            "<b>Version 1.0</b><br>"
            "<p align='center'>Created for course<br>"
            "<a href='http://www.cs.tut.fi/~projekti/'>TIE-13100 Project Work on Pervasive Systems</a><br>"
            "Tampere University of Technology<br><br>"
            "Credits:<br>"
            "Ville Jokela<br>"
            "Juhani Järvinen<br>"
            "Longchuan Niu<br>"
            "Joonas Pessi<br>"
            "Heikki Sarkanen<br>"
            "Miao Zhao<br><br>"
            "License: MIT<br><br>"
            "<a href='https://github.com/joonaspessi/rotfl'>Sources on Github</a></p>";
    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle("About Roomba, The Fleet Management");
    aboutBox.setTextFormat(Qt::RichText);
    aboutBox.setText(aboutDescription);
    aboutBox.exec();
}

void MainWindow::action_AboutQt_triggered()
{
    QMessageBox::aboutQt(this, "About Qt");
}
