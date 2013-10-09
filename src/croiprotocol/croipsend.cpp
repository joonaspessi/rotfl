#include "croipsend.h"
#include "croippacket.h"
#include "croip.h"

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

CroipSend::CroipSend():
    fdsocket_(-1),
    ip_(IP_DEFAULT),
    port_(PORT_DEFAULT)

{}

CroipSend::~CroipSend()
{
    deinit();
}

int CroipSend::init()
{
    fdsocket_ = socket(AF_INET,SOCK_STREAM, 0);

    if(fdsocket_ < 0){
        return -1;
    }

    struct sockaddr_in srvAddr;

    memset((char *)&srvAddr,0,sizeof(sockaddr_in));

    srvAddr.sin_family = AF_INET;
    int ret = inet_pton(AF_INET,ip_.c_str(),&srvAddr.sin_addr);

    if (ret <= 0 ){
        deinit();
        return -1;
    }

    srvAddr.sin_port = htons(port_);

    if( connect(fdsocket_,(struct sockaddr *)&srvAddr, sizeof(srvAddr)) < 0 ){
        deinit();
        perror("connect()");
        return -1;
    }

    return 0;

}

int CroipSend::deinit()
{
    if(fdsocket_ > 0) {
        if ( close(fdsocket_) < 0 ){
            perror("close()");
            return -1;
        }

    }
    return 0;
}

int CroipSend::send(CroipPacket &packet)
{
    unsigned char * message[MAX_PACKET_SIZE] = {0};
    memset(message[0],0,MAX_PACKET_SIZE);


    int size = packet.serialize(message[0]);

    if(size < 1) {
        return -1;
    }

    Util::writen(fdsocket_,message[0],size);

    return 0;

}

} //namespace Croip
