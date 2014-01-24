#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QGraphicsScene>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QPushButton>
#include <QToolButton>
#include "croi/posixSerial.h"
#include "croi/iRoomba.h"
#include "mapQGraphicsView.h"
#include "uiUtils.h"

class FleetManager;
class QVBoxLayout;
class QTabWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    void setCurrentFile(const QString &fileName);
    // Added asking of saving changes made to map before closing
    void closeEvent(QCloseEvent *event);
    // Tracks mouse movements and adds coordinates on map to statusbar
    bool eventFilter(QObject *object, QEvent *event);
    // Adds new tab for tabwidget_ for new roomba
    void addRoombaTab(Croi::IRoomba *roomba);
public slots:

    // Signal indicates that wall or point of interest is added to the map
    void mapModified();
    // Changes the tab corresponding to roomba
    void setSelectedRoombaTab();
private slots:

    void setRoombaStatusData();

    void pushButton_Connection_clicked();

    void pushButton_Clean_clicked();

    void pushButton_allMotors_clicked();

    void pushButton_playSong_clicked();

    void pushButton_tracesDisable_clicked();

    void pushButton_resetAngle_clicked();

    void toolButton_correctLeft_clicked();

    void toolButton_correctRight_clicked();

    void toolButton_correctUp_clicked();

    void toolButton_correctDown_clicked();

    void pushButton_correctCw_clicked();

    void pushButton_correctCcw_clicked();
    void toolButton_driveForward_clicked();

    void toolButton_driveBackward_clicked();

    void pushButton_turnCw_clicked();

    void pushButton_turnCcw_clicked();

    void velocity_horizontalSlider_sliderMoved(int position);

    void mapScale_horizontalSlider_sliderMoved(int position);

    void pushButton_Go2POIs_clicked();

    void pushButton_fleetManagementEnable_clicked();

    void action_Cursor_toggled(bool toggleStatus);

    void action_Wall_toggled(bool toggleStatus);
    void action_ATC_toggled(bool toggleStatus);

    void action_Poi_toggled(bool toggleStatus);

    void action_Start_toggled(bool toggleStatus);
    void action_StartVirtual_toggled(bool toggleStatus);
    void actionSave_triggered();
    void actionSaveAs_triggered();
    void actionOpen_triggered();
    void tabChanged_triggered(int index);
    void action_About_triggered();
    void action_AboutQt_triggered();
    void connectionEstablished();
    void connectionFailed();
signals:
    
protected:
    void keyPressEvent(QKeyEvent *event);
private:
    void init();
    void saveToFile(QString &fileName);
    void openFile(const QString &fileName);
    Ui::MainWindow *ui;
    FleetManager* fleetManager_;
    QTimer *updateSensorData_;
    MapQGraphicsView* map_;
    void createMenuBar();
    void createFleetManagementTab();
    void createRoombaDashBoardTab(QString name);
    void createToolbar();
    // Disables or enables UI elements on state change, state is true if changing to connected state
    void handleUIElementsConnectionStateChange(bool state);
    // Disables or enables UI elements on state change, state is true if changing to Fleet Management state
    void handleUIElementsControlModeStateChange(bool state);
    // Unchecks all manual driving buttons of selectedRoomba_
    void handleUIElementsDrivingStateChange();
    // Disables or enables UI elements on state change, state is true if changing to connecting state
    void handleUIElementsChangeAllTabsState(bool state);
    void stopAllManuallyControlledRoombas();
    void resetRoombaStatusInfo();
    QVBoxLayout *connect_layout_;
    QLabel *velocityValue_label_;
    QLabel *mapScaleValue_label_;
    QSlider *velocity_horizontalSlider_;
    QSlider *mapScale_horizontalSlider_;
    QWidget *connection_Widget_;
    QWidget *fleetManagement_Widget_;
    QString currentFile_;
    bool saveUnsavedChanges();
    Croi::IRoomba * selectedRoomba_;
    QPushButton *fleetManagementEnable_pushButton_;
    QPushButton *Go2POIs_pushButton_;
    QPushButton *clean_pushButton_;
    QTimer *updateRoombaStatusData_;
    // Toolbar Actions
    QAction *cursor_action_;
    QAction *wall_action_;
    QAction *start_action_;
    QAction *startVirtual_action_;

    QTabWidget *tabWidget_; // Includes all tabs
    QVector<QWidget*> roomba_Widgets_;
    // Roomba tab contents
    QMap<int,Croi::IRoomba*> roombaTabs_; // tab index, roomba pointer
    QMap<Croi::IRoomba*, QLineEdit *>ip1LineEdits_;
    QMap<Croi::IRoomba*, QLineEdit *>ip2LineEdits_;
    QMap<Croi::IRoomba*, QLineEdit *>ip3LineEdits_;
    QMap<Croi::IRoomba*, QLineEdit *>ip4LineEdits_;
    QMap<Croi::IRoomba*, QPushButton *>connection_pushButtons_;
    QMap<Croi::IRoomba*, QPushButton *>allMotors_pushButtons_;
    QMap<Croi::IRoomba*, QPushButton *>playSong_pushButtons_;
    QMap<Croi::IRoomba*, QToolButton *>driveForward_toolButtons_;
    QMap<Croi::IRoomba*, QToolButton *>driveBackward_toolButtons_;
    QMap<Croi::IRoomba*, QPushButton *>turnCw_pushButtons_;
    QMap<Croi::IRoomba*, QPushButton *>turnCcw_pushButtons_;
    QMap<Croi::IRoomba*, QPushButton *>resetAngle_pushButtons_;
    QMap<Croi::IRoomba*, QToolButton *>correctLeft_toolButtons_;
    QMap<Croi::IRoomba*, QToolButton *>correctRight_toolButtons_;
    QMap<Croi::IRoomba*, QToolButton *>correctUp_toolButtons_;
    QMap<Croi::IRoomba*, QToolButton *>correctDown_toolButtons_;
    QMap<Croi::IRoomba*, QPushButton *>correctCw_pushButtons_;
    QMap<Croi::IRoomba*, QPushButton *>correctCcw_pushButtons_;
    QMap<Croi::IRoomba*, QLabel *>temperature_labels_;
    QMap<Croi::IRoomba*, QLabel *>rmbPosition_labels_;
    QMap<Croi::IRoomba*, QLineEdit *>roombaNameLineEdits_;
    QMap<Croi::IRoomba*, QObject *>roombaStatuses_;
};

#endif // MAINWINDOW_H
