#include "roombaRoowifi.h"
#include "croiUtil.h"
#include "mapQGraphicsView.h"

namespace Croi {

RoombaRoowifi::RoombaRoowifi(PoiQGraphicsEllipseItem *startPoint,
                             MapQGraphicsView* map, FleetManager *parent):
    IRoomba(startPoint, map, parent)
{
    roowifi_ = new RooWifi(this);
    //TODO: implement own timer
    connect(roowifi_,SIGNAL(AutoCaptureUpdated()), qobject_cast<IRoomba*>(this), SLOT(sensorUpdateTimerTimeout()));
    IRoomba::updateState(); //call needed to have no null pointer in polygon_
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
    //TODO: implement own timer
    roowifi_->SetAutoCaptureTime(500);
    roowifi_->StartAutoCapture();
}

int RoombaRoowifi::disconnect()
{
    roowifi_->StopAutoCapture();
    roowifi_->Disconnect();
    IRoomba::disconnect();
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
    IRoomba::safeMode();
}

void RoombaRoowifi::fullMode()
{
    roowifi_->FullMode();
    IRoomba::fullMode();
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

double RoombaRoowifi::getDistance()
{
    return roowifi_->Sensors.Distance;
}

double RoombaRoowifi::getAngle()
{
    return roowifi_->Sensors.Angle;
}

bool RoombaRoowifi::getLeftBumb()
{
    unsigned char bumps = roowifi_->Sensors.BumpsWheeldrops;
    if (bumps & 0x02)
    {
        return true;
    }
    return false;
}

bool RoombaRoowifi::getRightBumb()
{
    unsigned char bumps = roowifi_->Sensors.BumpsWheeldrops;
    if (bumps & 0x01)
    {
        return true;
    }
    return false;
}

void RoombaRoowifi::drive(int velocity, int radius)
{
    roowifi_->Drive(velocity, radius);
    IRoomba::drive(velocity, radius);
}

void RoombaRoowifi::drive(int velocity)
{
    roowifi_->Drive(velocity, getRadius());
    IRoomba::drive(velocity);
}

void RoombaRoowifi::playSong(int songNumber)
{
    roowifi_->PlaySong(songNumber);
}


} //namespace Croi
