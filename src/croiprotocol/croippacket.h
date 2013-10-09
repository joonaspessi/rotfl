#ifndef CROIPPACKETWRAP_H
#define CROIPPACKETWRAP_H

#include <cstdlib>

namespace Croip {

class CroipPacket
{
public:
    CroipPacket();
    virtual ~CroipPacket();

    virtual int serialize(unsigned char * const buf) const;
    virtual int deserialize(char * const buf);

private:
    u_int32_t type_;
    char testibuffer_[255];
    u_int32_t optCode_;
};

} //namespace Croip

#endif // CROIPPACKETWRAP_H
