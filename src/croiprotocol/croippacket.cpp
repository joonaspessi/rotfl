#include <iostream>
#include <cstring>
#include "croippacket.h"
#include "croip.h"

namespace Croip {


CroipPacket::CroipPacket()
{
}

CroipPacket::~CroipPacket()
{

}

int CroipPacket::serialize(unsigned char * const buf) const
{
    u_int16_t count = 0;

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

    //OPERATION CODE
    buf[count++] = (optCode_>>24) & 0xFF;
    buf[count++] = (optCode_ >> 16) & 0xFF;
    buf[count++] = (optCode_ >> 8) & 0xFF;
    buf[count++] = (optCode_) & 0xFF;

    //set size
    u_int16_t size = 0;
    size = count -2;

    buf[0] = (size >> 8) & 0xFF;
    buf[1] = (size) & 0xFF;




    return count;

}

int CroipPacket::deserialize(char * const buf)
{
    u_int16_t count = 0;

    //

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
    for(int i = 0; i < 7; ++i) {
        testibuffer_[i] = buf[i];
    }

    return 0;

}

} //namespace Croip
