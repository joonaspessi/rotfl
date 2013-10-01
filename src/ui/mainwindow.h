#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "croi/posixSerial.h"

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

signals:
    void allPoisCleared();
    
private:
    void init();
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    int index;
    Croi::PosixSerial* posixserial;
    ThreadReader *threadReader;
};

#endif // MAINWINDOW_H
