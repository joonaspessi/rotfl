#include "croip.h"
#include <cerrno>

extern "C" {
#include <unistd.h>

}

namespace Croip {

Croip::Croip()
{
}

Croip::~Croip()
{

}


namespace Util {
ssize_t readn(int fd, void *buffer, size_t count) {

    ssize_t numRead = 0;
    size_t totRead = 0;
    char *buf;

    buf = (char *)buffer;

    for(totRead = 0; totRead < count;) {

        numRead = read(fd, buffer, count - totRead);

        if(numRead == 0) { //EOF
            return totRead;
        }

        if(numRead == -1) {
            if(errno == EINTR) {
                continue;
            }
            else {
                perror("readn: read()");
            }
        }

        totRead += numRead;
        buf += numRead;

    }
    return totRead;
}

ssize_t writen(int fd, void *buffer, size_t count) {

    ssize_t numWritten;
    size_t totWritten;

    const char *buf;
    buf = (char *)buffer;

    for (totWritten = 0; totWritten < count; ) {
        numWritten = write(fd, buf, count - totWritten);
        if (numWritten <= 0) {
            if (numWritten == -1 && errno == EINTR)
                continue;

            else
                perror("writen: write()");
                return -1;

        }
        totWritten += numWritten;
        buf += numWritten;
    }
    return totWritten;


}
} //namespace Util

} //namespace Croip
