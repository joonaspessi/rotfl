#include <QListView>
#include <QDebug>
#include <QString>
#include <iomanip>
#include <sstream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qfunctions_wince.h>
#include "threadReader.h"


MainWindow::MainWindow(QWidget *parent) :
    index(0), QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new QStandardItemModel(3, 3);
    ui->listView->setModel(model);
    posixserial = new CROI::PosixSerial();

    //threadReader = new ThreadReader(posixserial, this);
    //threadReader->start();
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString send;

    send += ui->lineEdit->text();

    bool isConversionOk = false;

    char optCommand = static_cast<char>(send.toInt(&isConversionOk));

    std::stringstream stream;
    stream << std::hex << optCommand;
    std::string optCommands(stream.str());
    posixserial->writeSerial(optCommands);


/*
    ++index;
    QStandardItem *item = new QStandardItem(QString("Start send"));
    model->setItem(index, 0, item);
    std::string buf;
    qDebug() << posixserial->readSerial(buf) ;
    qDebug() << QString::fromStdString(buf);
//    qDebug() << posixserial->writeSerial("morooo");
*/

}


void MainWindow::on_pushButton_2_clicked()
{
    std::string buf;
    posixserial->readSerial(buf) ;
    if(buf.size() > 0) {
        QStandardItem *item = new QStandardItem(QString(buf.c_str()));
        model->setItem(index, 0, item);
        index++;

    }

}


