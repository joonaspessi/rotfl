#include <QApplication>
#include <QObject>
#include "mainwindow.h"
#include "ui_mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("fusion");
    a.setApplicationDisplayName("Roomba, The Fleet Management");
    MainWindow w;
    w.show();
    return a.exec();
}
