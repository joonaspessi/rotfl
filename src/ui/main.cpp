#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QObject>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("fusion");
    a.setApplicationDisplayName("Roomba, The Fleet Management");
    MainWindow w;
    w.show();
    return a.exec();
}
