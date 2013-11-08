#include "roombaRoowifi.h"
#include "croiUtil.h"

namespace Croi {

RoombaRoowifi::RoombaRoowifi(poiQGraphicsEllipseItem *startPoint, QObject *parent):
    IRoomba(startPoint, parent)
{
    roowifi_ = new RooWifi(this);
    connect(roowifi_,SIGNAL(AutoCaptureUpdated()), parent, SLOT(sensorUpdateTimerTimeout()));
}

RoombaRoowifi::~RoombaRoowifi()
{
    roowifi_->StopAutoCapture();
    roowifi_->Disconnect();
}

int RoombaRoowifi::rmb_connect(std::string ip)
{
    QString qip = QString::fromStdString(ip);
    qDebug() << "set ip to:" << qip;
    roowifi_->SetIP(qip);
    roowifi_->Connect();
    roowifi_->StartAutoCapture();
    roowifi_->SetAutoCaptureTime(500);
}

int RoombaRoowifi::disconnect()
{
    roowifi_->StopAutoCapture();
    roowifi_->Disconnect();
}

void RoombaRoowifi::safeMode()
{
    roowifi_->SafeMode();
    int Song[14];
    int SongDuration[14];

    //Pop Corn part1
    Song[0] = 82;   SongDuration[0] = RooWifi::NotesDuration::SemiQuaver;
    Song[1] = 80;   SongDuration[1] = RooWifi::NotesDuration::SemiQuaver;
    Song[2] = 82;   SongDuration[2] = RooWifi::NotesDuration::SemiQuaver;
    Song[3] = 77;   SongDuration[3] = RooWifi::NotesDuration::SemiQuaver;
    Song[4] = 73;   SongDuration[4] = RooWifi::NotesDuration::SemiQuaver;
    Song[5] = 75;   SongDuration[5] = RooWifi::NotesDuration::SemiQuaver;
    Song[6] = 70;   SongDuration[6] = RooWifi::NotesDuration::Quaver;
    Song[7] = 82;   SongDuration[7] = RooWifi::NotesDuration::SemiQuaver;
    Song[8] = 80;   SongDuration[8] = RooWifi::NotesDuration::SemiQuaver;
    Song[9] = 82;   SongDuration[9] = RooWifi::NotesDuration::SemiQuaver;
    Song[10] = 77;  SongDuration[10] = RooWifi::NotesDuration::SemiQuaver;
    Song[11] = 73;  SongDuration[11] = RooWifi::NotesDuration::SemiQuaver;
    Song[12] = 75;  SongDuration[12] = RooWifi::NotesDuration::SemiQuaver;
    Song[13] = 70;  SongDuration[13] = RooWifi::NotesDuration::Quaver;

    roowifi_->StoreSong( 1, 14, Song, SongDuration );
}

void RoombaRoowifi::fullMode()
{
    roowifi_->FullMode();
}

void RoombaRoowifi::allMotorsOn()
{
    roowifi_->AllCleaningMotors_On();
}

void RoombaRoowifi::allMotorsOff()
{
    roowifi_->AllCleaningMotors_Off();
}

void RoombaRoowifi::clean()
{
    roowifi_->Clean();
}

char RoombaRoowifi::getTemperature()
{
    return roowifi_->Sensors.Temperature;
}

unsigned short RoombaRoowifi::getChargeLevel()
{
    return roowifi_->Sensors.Charge;
}

short RoombaRoowifi::getDistance()
{
    return roowifi_->Sensors.Distance;
}

short RoombaRoowifi::getAngle()
{
    return roowifi_->Sensors.Angle;
}

void RoombaRoowifi::Drive(int Velocity, int Radius)
{
    roowifi_->Drive(Velocity, Radius);
    IRoomba::Drive(Velocity, Radius);
}

void RoombaRoowifi::playSong(int songNumber)
{
    roowifi_->PlaySong(songNumber);
}

void RoombaRoowifi::testSlot()
{
    qDebug() << "autocapture";
    qDebug() << "temperature" << QString::number((char) roowifi_->Sensors.Temperature );
    qDebug() << "dirt right" << QString::number(roowifi_->Sensors.DirtDetectorRight );
}

} //namespace Croi
