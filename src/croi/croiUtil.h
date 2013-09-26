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
    {START,     {START, 0, "start"}},
    {BAUD,      {BAUD, 1, "baud"}},
    {CONTROL,   {CONTROL, 0, "control"}},
    {SAFE,      {SAFE, 0, "safe"}}
    //TODO add all optcodes

};


} //namespace Croi

#endif // CROIUTIL_H
