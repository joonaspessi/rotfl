#ifndef CROIUTIL_H
#define CROIUTIL_H

#include <map>
#include <string>

namespace Croi {

const double ANGLECORRECTION = 3.057;
const double DISTANCECORRECTION = 6.1;
const double PI = 3.14159265;
const unsigned int FWSPEED = 500;  //this DOES NOT affect turning speed (100)
//Roomba turns 1 degree in 18055 microseconds, when speed is 100
const unsigned int TURNTIMEINUSFORDEG = 18055;


enum returnval {
    CROI_ERROR = -1,
    CROI_SUCCESS = 0
};

enum OpCode {
    START = 128,
    BAUD = 129,
    CONTROL = 130,
    SAFE = 131,
    FULL = 132,
    POWER = 133,
    SPOT = 134,
    CLEAN = 135,
    MAX = 136,
    DRIVE = 137,
    DRIVE_WHEELS = 145,
    MOTORS = 138,
    PWM_MOTORS = 144,
    DRIVE_PWM = 146,
    LEDS = 139,
    SONG = 140,
    PLAY = 141,
    STREAM = 148,
    QUERY_LIST = 149,
    DO_STREAM = 150,
    QUERY = 142,
    FORCE_SEEKING_DOCK = 143,
    SCHEDULING_LEDS = 162,
    DIGIT_LEDS_RAW = 163,
    DIGIT_LEDS_ASCII = 164,
    BUTTONS = 165,
    SCHEDULE = 167,
    SET_DAY_TIME = 168
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
    {POWER,             {POWER, 0, "power" }},
    {SPOT,              {SPOT, 0, "spot"}},
    {CLEAN,             {CLEAN, 0, "clean"}},
    {MAX,               {MAX, 0, "max"}},
    {DRIVE,             {DRIVE, 4, "drive"}},
    {DRIVE_WHEELS,      {DRIVE_WHEELS, 4, "drive wheels"}},
    {MOTORS,            {MOTORS, 1, "motors"}},
    {PWM_MOTORS,        {PWM_MOTORS, 3, "pwm motors"}},
    {DRIVE_PWM,         {DRIVE_PWM, 4, "drive pwm"}},
    {LEDS,              {LEDS, 3, "leds"}},
    {SONG,              {SONG, 2, "song"}},
    {PLAY,              {PLAY, 1, "play"}},
    {STREAM,            {STREAM, 1, "stream"}},
    {QUERY_LIST,        {QUERY_LIST, 1, "query list"}},
    {DO_STREAM,         {DO_STREAM, 1, "do stream"}},
    {QUERY,             {QUERY, 1, "query"}},
    {FORCE_SEEKING_DOCK,{FORCE_SEEKING_DOCK, 0, "force seeking dock"}},
    {SCHEDULING_LEDS,   {SCHEDULING_LEDS, 2, "scheduling leds"}},
    {DIGIT_LEDS_RAW,    {DIGIT_LEDS_RAW, 4, "digit leds raw"}},
    {DIGIT_LEDS_ASCII,  {DIGIT_LEDS_ASCII, 4, "digit leds ascii"}},
    {BUTTONS,           {BUTTONS, 1, "buttons"}},
    {SCHEDULE,          {SCHEDULE, 15, "schedule"}},
    {SET_DAY_TIME,      {SET_DAY_TIME, 3, "set day time"}}
};


} //namespace Croi

#endif // CROIUTIL_H
