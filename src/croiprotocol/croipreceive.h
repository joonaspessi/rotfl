#ifndef CROIPRECEIVE_H
#define CROIPRECEIVE_H

#include <vector>
#include <mutex>
#include "croippacket.h"

namespace Croip {



class croipReceive
{
public:
    croipReceive();

    virtual ~croipReceive();

    int init();

    int deinit();

    void recvLoop();

    void getPackets(std::vector<CroipPacket> packets);

private:
    std::vector<CroipPacket>recvPackets_;
    std::mutex packetMutex_;
    int fdsocket_;
    u_int16_t port_;
};

} //namespace Croip

#endif // CROIPRECEIVE_H
