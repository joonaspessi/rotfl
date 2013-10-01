#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include "mapQGraphicsView.h"
#include <QObject>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    QObject::connect(&w, &MainWindow::allPoisCleared,
                     w.giveUiPointer()->mapView,
                     &mapQGraphicsView::clearAllPois);
    return a.exec();
}
