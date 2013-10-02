#ifndef CROIP_H
#define CROIP_H

#include <string>
#include <cstdlib>

namespace Croip {

const u_int16_t PORT_DEFAULT = 1337;
const std::string IP_DEFAULT = "192.168.1.2";

const u_int32_t MAX_PACKET_SIZE = 2000;

const char VERSION_MAJOR = '0';
const char VERSION_MINOR = '0';
const char VERSION_PATCH = '1';


namespace Util {
ssize_t readn(int fd, void *buffer, size_t count);
ssize_t writen(int fd, void *buffer, size_t count);
}

class Croip
{

public:
    Croip();
    virtual ~Croip();
};

} //namespace Croip
#endif // CROIP_H
