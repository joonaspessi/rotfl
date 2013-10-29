#include <QListView>
#include <QDebug>
#include <QString>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <QDateTime>
#include <QGridLayout>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qfunctions_wince.h>
#include "threadReader.h"

#include "croi/iRoomba.h"
#include "croi/roombaSerial.h"
#include "croi/roombaRoowifi.h"
#include "croi/croiUtil.h"
#include "mapQGraphicsView.h"

MainWindow::MainWindow(QWidget *parent) :
    index(0), moving_(false), QMainWindow(parent),
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

//    Disabled until Roowifi AutoCapture is used instead
//    updateSensorData_ = new QTimer(this);
//    connect(updateSensorData_,SIGNAL(timeout()),this,SLOT(sensorUpdateTimerTimeout()));

    iRoomba_ = new Croi::RoombaRoowifi(this);

    //threadReader = new ThreadReader(posixserial, this);
    //threadReader->start();

    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));

    QString temp;
    //show the default real world width of map in cm
    ui->lineEdit_mapWidth->setText(temp.setNum(ui->mapView->giveMapWidth()));

    QAction* quitAct = new QAction(tr("&Quit"),this);
    quitAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    connect(quitAct, SIGNAL(triggered()),this,SLOT(close()));
    fileMenu->addAction(quitAct);
}

MainWindow::~MainWindow()
{
    releaseKeyboard();
    delete ui;
}

Ui::MainWindow* MainWindow::giveUiPointer()
{
    return ui;
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


    //send message to ui
    QString outputStr = "";
    for(int i = 0; i < optCommands.size(); ++i){
        QByteArray ba = 0;
        ba.append(optCommands.at(i));
        outputStr += ba.toHex();
        outputStr += " ";
    }

    QString outputCommandUI;
    outputCommandUI = QTime::currentTime().toString() + " Write: " + outputStr ;
    QStandardItem *item = new QStandardItem(outputCommandUI);
    model->setItem(index, 0, item);
    index++;
    ui->listView->scrollToBottom();

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
        ui->listView->scrollToBottom();
    }

}

void MainWindow::on_pushButton_3_clicked()
{
    ui->mapView->removeRedObjects();
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
        ui->byte_4_edit->show();
        break;
    }
    default:
        ui->byte_1_edit->hide();
        ui->byte_2_edit->hide();
        ui->byte_3_edit->hide();
        ui->byte_4_edit->hide();
    }

    ui->byte_1_edit->clear();
    ui->byte_2_edit->clear();
    ui->byte_3_edit->clear();
    ui->byte_4_edit->clear();


}

void MainWindow::on_pushButton_Connect_clicked()
{
    grabKeyboard();
//    Disabled until Roowifi AutoCapture is used instead
//    updateSensorData_->start(500);
    QString ip = ui->ipLineEdit_1->text() + "." + ui->ipLineEdit_2->text() + "." + ui->ipLineEdit_3->text()
            + "." + ui->ipLineEdit_4->text();
    std::string stdip = ip.toStdString();
    iRoomba_->rmb_connect(stdip);
}

void MainWindow::on_pushButton_Disconnect_clicked()
{
    releaseKeyboard();
//    Disabled until Roowifi AutoCapture is used instead
//    updateSensorData_->stop();
    iRoomba_->disconnect();
    ui->temperature_label->setText("0");
    ui->charge_label->setText("0");
    ui->velocity_horizontalSlider->setValue(0);
    ui->velocityValue_label->setText("0");
}

void MainWindow::on_pushButton_Clean_clicked()
{
    iRoomba_->clean();
}

void MainWindow::on_pushButton_allMotorsOn_clicked()
{
    iRoomba_->allMotorsOn();
}

void MainWindow::on_pushButton_allMotorsOff_clicked()
{
    iRoomba_->allMotorsOff();
}

void MainWindow::on_pushButton_Safe_clicked()
{
    iRoomba_->safeMode();
}

void MainWindow::on_pushButton_Full_clicked()
{
    iRoomba_->fullMode();
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    //Remember following special radiuses
    //Straight = 32768 or 32767
    //Turn in place clockwise = 65535
    //Turn in place counter-clockwise = 1
    qDebug() << "KeyPress";
    if(event->key() == Qt::Key_W) {
        iRoomba_->Drive(ui->velocity_horizontalSlider->value(),32767);
        radius_ = 32767;
        moving_ = true;
        qDebug() << "UpArrow";
    }
    else if(event->key() == Qt::Key_A) {
        iRoomba_->Drive(ui->velocity_horizontalSlider->value(),200);
        radius_ = 200;
        moving_ = true;
        qDebug() << "RightArrow";
     }
    else if(event->key() == Qt::Key_D) {
        iRoomba_->Drive(ui->velocity_horizontalSlider->value(),-200);
        radius_ = -200;
        moving_ = true;
        qDebug() << "LeftArrow";
    }
    else if(event->key() == Qt::Key_E) {
        iRoomba_->Drive(ui->velocity_horizontalSlider->value(),65535);
        radius_ = 65535;
        moving_ = true;
        qDebug() << "Turn clockwise";
    }
    else {
        iRoomba_->Drive(0,32767);
        radius_ = 32767;
        moving_ = false;
        qDebug() << "Stop";
    }
}

void MainWindow::sensorUpdateTimerTimeout()
{
//    qDebug() << "sensorUpdateTimerTimeout";
    ui->temperature_label->setText( QString::number( ( unsigned char )( iRoomba_->getTemperature() ) ) );
    ui->charge_label->setText( QString::number( (unsigned short)( iRoomba_->getChargeLevel() ) ) );
    ui->mapView->updateLoc(iRoomba_->getDistance(), iRoomba_->getAngle(), iRoomba_->getRadius(),
                           iRoomba_->getVelocity());
}

void MainWindow::on_pushButton_playSong_clicked()
{
    iRoomba_->playSong(1);
}

void MainWindow::on_pushButton_unshowTraces_clicked()
{
    ui->mapView->ifShowTraces();
}

void MainWindow::on_pushButton_simMov_clicked()
{
    double distance = -rand()%500;
    double angle = -(rand()%90-rand()%90);
    ui->mapView->updateLoc(distance, angle/3.05, static_cast<int>(2000*(360-angle)/360),
                           rand()%500);
    //ui->mapView->updateLoc(-300, -10, 1, rand()%500);  //simple version
}

void MainWindow::on_velocity_horizontalSlider_sliderMoved(int position)
{
    ui->velocityValue_label->setText(QString::number(position));
    if (moving_) {
        iRoomba_->Drive(position,radius_);
    }
}

void MainWindow::on_pushButton_mapWidth_clicked()
{
    ui->mapView->changeMapWidth(ui->lineEdit_mapWidth->text().toInt());
}

void MainWindow::on_pushButton_resetAngle_clicked()
{
    ui->mapView->resetAngle();
}
