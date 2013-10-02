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

    //PACKET HEADER
    buf[count++] = 'C';
    buf[count++] = 'R';
    buf[count++] = 'O';
    buf[count++] = 'I';

    //VERSION
    buf[count++] = VERSION_MAJOR;
    buf[count++] = VERSION_MINOR;
    buf[count++] = VERSION_PATCH;

    //SIZE



    return count;

}

int CroipPacket::deserialize(char * const buf) const
{

    return 0;

}

} //namespace Croip
