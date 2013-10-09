#include <iostream>
#include "croippacket.h"
#include "croip.h"

namespace Croip {


CroipPacket::CroipPacket()
{
}

CroipPacket::~CroipPacket()
{

}

int CroipPacket::serialize(char * const buf) const
{
    u_int32_t count = 0;

    for(int i = 0; i < 7; ++i){
        buf[i] = testibuffer_[i];
    }
    return 7;
    //SIZE
    buf[count++] = 0x00;
    buf[count++] = 0x00;

    //PACKET HEADER
    buf[count++] = HEADER_1;
    buf[count++] = HEADER_2;
    buf[count++] = HEADER_3;
    buf[count++] = HEADER_4;

    //VERSION
    buf[count++] = VERSION_MAJOR;
    buf[count++] = VERSION_MINOR;
    buf[count++] = VERSION_PATCH;




    return count;

}

int CroipPacket::deserialize(char * const buf)
{
    u_int32_t count = 0;

    //PACKET HEADER
    if(buf[count++] != HEADER_1 ||
       buf[count++] != HEADER_2 ||
       buf[count++] != HEADER_3 ||
       buf[count++] != HEADER_4)
    { return -1; }

    //VERSION
    if(buf[count++] != VERSION_MAJOR ||
       buf[count++] != VERSION_MINOR ||
       buf[count++] != VERSION_PATCH)
    { return -1; }



    //PACKET SIZE
    for(int i = 0; i < 7; ++i){
        testibuffer_[i] = buf[i];
    }

    return 0;

}

} //namespace Croip
