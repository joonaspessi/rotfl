#include "croipreceive.h"
#include "croip.h"

#include <iostream>
#include <cstring>
#include <cerrno>

extern "C" {
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
}

namespace Croip {

croipReceive::croipReceive() :
    fdsocket_(-1),
    port_(PORT_DEFAULT)
{
}

croipReceive::croipReceive(u_int16_t port) :
    fdsocket_(-1),
    port_(port)

{

}

croipReceive::~croipReceive()
{

}

int croipReceive::init()
{
    struct sockaddr_in srvAddr;
    memset((char *)&srvAddr,0,sizeof(sockaddr_in));



    fdsocket_ = socket(PF_INET, SOCK_STREAM, 0);
    if(fdsocket_ < 0) {
        perror("init: socket()");
        return -1;
    }

    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    srvAddr.sin_port = htons(port_);



    if( bind(fdsocket_, (struct sockaddr*) &srvAddr, sizeof(srvAddr)) < 0 ) {
        perror("init: bind()");
        return -1;
    }

    listen(fdsocket_, 5); //Magic five :)

    return 0;

}

int croipReceive::deinit()
{
    if(fdsocket_ > 0) {
        if( close(fdsocket_) < 0) {
            perror("deinit: close()");
            return -1;
        }

    }
    return 0;

}

void croipReceive::recvLoop()
{
    struct sockaddr_in clientAddr;
    memset((char *)&clientAddr,0,sizeof(sockaddr_in));
    socklen_t clilen = sizeof(clientAddr);

    int newsockfd = accept(fdsocket_,(struct sockaddr*) &clientAddr, &clilen );

    if(newsockfd < 0) {
        perror("init: accept()");
        return;
    }

    char message[MAX_PACKET_SIZE];


    while(true) {
        if( Util::readn(newsockfd,message,7) != 7 ) {
            perror("recvLoop : readn()");
            return;
        }
        std::cout << "new packet received..." << std::endl;

        message[7] = '\0';

        CroipPacket newpacket;
        newpacket.deserialize(message);

        addPacket(newpacket);
    }

    if( close(newsockfd) < 0) {
        perror("recvloop: close");
    }

}

void croipReceive::getPackets(std::vector<CroipPacket> &packets)
{
    packetMutex_.lock();
    packets = recvPackets_;
    recvPackets_.clear();
    packetMutex_.unlock();
}

void croipReceive::addPacket(CroipPacket &packet)
{
    packetMutex_.lock();
    recvPackets_.push_back(packet);
    packetMutex_.unlock();

}

} //namespace Croip
