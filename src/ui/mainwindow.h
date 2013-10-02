#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "croi/posixSerial.h"
#include "../ext_lib/RooWifi_11b/roowifi.h"

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

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_pushButton_3_clicked();

    void on_pushButton_Connect_clicked();

    void on_pushButton_Disconnect_clicked();

    void on_pushButton_Clean_clicked();

    void on_pushButton_DriveForward_clicked();

    void on_pushButton_DriveBackward_clicked();

    void on_pushButton_Safe_clicked();

    void on_pushButton_Full_clicked();

    void on_pushButton_Stop_clicked();

signals:
    void allPoisCleared();
    
protected:
    void keyPressEvent(QKeyEvent *event);
private:
    void init();
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    int index;
    Croi::PosixSerial* posixserial;
    ThreadReader *threadReader;
    RooWifi* roowifi_;
    bool direction_;
};

#endif // MAINWINDOW_H
