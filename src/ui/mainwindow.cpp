#include <QListView>
#include <QDebug>
#include <QString>
#include <iomanip>
#include <sstream>
#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qfunctions_wince.h>
#include "threadReader.h"

#include "croi/iRoomba.h"
#include "croi/roombaSerial.h"
#include "croi/croiUtil.h"

MainWindow::MainWindow(QWidget *parent) :
    index(0), QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new QStandardItemModel(3, 3);
    ui->listView->setModel(model);
    posixserial = new Croi::PosixSerial();

    //combobox
    for(auto i = Croi::ROOMBA_COMMAND_MAP.begin(); i != Croi::ROOMBA_COMMAND_MAP.end(); ++i){
        ui->comboBox->addItem(tr(i->second.name.c_str()));
    }

    Croi::IRoomba* roombaSerial = new Croi::RoombaSerial();

    roombaSerial->connect();

    //threadReader = new ThreadReader(posixserial, this);
    //threadReader->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    std::stringstream stream;

    QString send;

    send += ui->lineEdit->text();

    bool isConversionOk = false;

    char optCommand = static_cast<char>(send.toInt(&isConversionOk));

    if(isConversionOk){
        stream << std::hex << optCommand;
    }

    if( ui->byte_1_edit->isHidden() == false) {
        QString temp;

        for(int i = 0; i < ui->byte_1_edit->text().size(); ++i){
            if(i > 1)
                break;
            QByteArray ba;
            ba.append( ui->byte_1_edit->text().at(i) );
            temp += ba;

        }


        stream << temp.toStdString();
    }



    std::string optCommands(stream.str());
    posixserial->writeSerial(optCommands);


}


void MainWindow::on_pushButton_2_clicked()
{
    std::string buf;
    posixserial->readSerial(buf);

    QString outputStr = "";
    if(buf.size() > 0){
        for(int i = 0; i < buf.size(); ++i){
            QByteArray ba = 0;
            ba.append(buf.at(i));
            outputStr += ba.toHex();
            outputStr += " ";
        }

        std::cout << "testi : " << outputStr.toStdString() << std::endl;
    }

    if(buf.size() > 0) {
        QStandardItem *item = new QStandardItem(outputStr);
        model->setItem(index, 0, item);
        index++;
    }

}



void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    std::cout << arg1.toStdString() << std::endl;
    int optCode = 0;
    int bytes = 0;
    for(auto i = Croi::ROOMBA_COMMAND_MAP.begin(); i != Croi::ROOMBA_COMMAND_MAP.end(); ++i) {
        if( i->second.name == arg1.toStdString() ){
            optCode = i->second.opcode;
            bytes = i->second.bytes;
            break;
        }
    }

    if( optCode != 0) {
        std::cout << optCode << std::endl;
        ui->lineEdit->setText(QString::number(optCode));
    }

    switch (bytes)
    {
    case 0:
    {
        ui->byte_1_edit->hide();
        ui->byte_2_edit->hide();
        ui->byte_3_edit->hide();
        ui->byte_4_edit->hide();
        break;
    }
    case 1:
    {
        ui->byte_1_edit->show();
        ui->byte_2_edit->hide();
        ui->byte_3_edit->hide();
        ui->byte_4_edit->hide();
        break;
    }
    case 2:
    {
        ui->byte_1_edit->show();
        ui->byte_2_edit->show();
        ui->byte_3_edit->hide();
        ui->byte_4_edit->hide();
        break;
    }
    case 3:
    {
        ui->byte_1_edit->show();
        ui->byte_2_edit->show();
        ui->byte_3_edit->show();
        ui->byte_4_edit->hide();
        break;
    }
    case 4:
    {
        ui->byte_1_edit->show();
        ui->byte_2_edit->show();
        ui->byte_3_edit->show();
        ui->byte_4_edit->hide();
        break;
    }
    }

}
