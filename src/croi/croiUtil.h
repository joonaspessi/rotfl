#ifndef CROIUTIL_H
#define CROIUTIL_H

#include <map>

namespace Croi {

enum returnval {
    CROI_ERROR = -1,
    CROI_SUCCESS = 0
};

enum OpCode {
    START = 128,
    BAUD,
    CONTROL,
    SAFE,
    FULL,
    SPOT,
    COVER,
    DEMO,
    DRIVE,
    LOW_SIDE_DRIVERS,
    LEDS,
    SONG,
    PLAY,
    SENSORS,
    COVER_AND_DOCK,
    PWM_LOW_SIDE_DRIVERS,
    DRIVE_DIRECT,
    DIGITAL_OUTPUTS,
    STREAM,
    QUERY_LIST,
    PAUSE_RESUME_STREAM,
    SEND_IR,
    SCRIPT,
    PLAY_SCRIPT,
    WAIT_TIME,
    WAIT_DISTANCE,
    WAIT_ANGLE,
    WAIT_EVENT
};

struct RoombaCommand {
    OpCode opcode;
    unsigned int bytes;
    std::string name;
};

const std::map<int, RoombaCommand> ROOMBA_COMMAND_MAP = {
    {START,             {START, 0, "start"}},
    {BAUD,              {BAUD, 1, "baud"}},
    {CONTROL,           {CONTROL, 0, "control"}},
    {SAFE,              {SAFE, 0, "safe"}},
    {FULL,              {FULL, 0, "full"}},
    {SPOT,              {SPOT, 0, "spot" }},
    {COVER,             {COVER, 0, "cover"}},
    {DEMO,              {DEMO, 1, "demo"}},
    {DRIVE,             {DRIVE, 4, "drive"}},
    {LOW_SIDE_DRIVERS,  {LOW_SIDE_DRIVERS, 1, "low side drivers"}},
    {LEDS,              {LEDS, 3, "leds"}},
    {SONG,              {SONG, 4, "song"}},
    {PLAY,              {PLAY, 1, "play"}},
    {SENSORS,           {SENSORS, 1, "sensors"}},
    {COVER_AND_DOCK,    {COVER_AND_DOCK, 0, "cover and dock"}},
    {PWM_LOW_SIDE_DRIVERS, {PWM_LOW_SIDE_DRIVERS, 3, "pwm low side drivers"}},
    {DRIVE_DIRECT,      {DRIVE_DIRECT, 4, "drive direct"}},
    {DIGITAL_OUTPUTS,   {DIGITAL_OUTPUTS, 1, "digital outputs"}},
    {STREAM,            {STREAM, 3, "stream"}},
    {QUERY_LIST,        {QUERY_LIST, 2, "query list"}},
    {PAUSE_RESUME_STREAM, {PAUSE_RESUME_STREAM, 1, "pause resume stream"}},
    {SEND_IR,           {SEND_IR, 1, "send ir"}},
    {SCRIPT,            {SCRIPT, 4, "script"}},
    {PLAY_SCRIPT,       {PLAY_SCRIPT, 0, "play script"}},
    {WAIT_TIME,         {WAIT_TIME, 1, "wait time"}},
    {WAIT_DISTANCE,     {WAIT_DISTANCE, 2, "wait distance"}},
    {WAIT_ANGLE,        {WAIT_ANGLE, 2, "wait angle"}},
    {WAIT_EVENT,        {WAIT_EVENT, 1, "wait event"}}

    //TODO add all optcodes

};


} //namespace Croi

#endif // CROIUTIL_H
