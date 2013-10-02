#ifndef CROIPSEND_H
#define CROIPSEND_H

#include <string>

namespace Croip {

class CroipPacket;

class CroipSend
{
public:
    CroipSend();

    virtual ~CroipSend();

    int init();

    int deinit();

    int send(CroipPacket& packet);

private:
    int fdsocket_;
    std::string ip_;
    uint16_t port_;
};

} //namespace Croip

#endif // CROIPSEND_H
