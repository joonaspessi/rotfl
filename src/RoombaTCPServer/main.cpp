#include <iostream>
#include <thread>


extern "C" {
#include <unistd.h>
}

#include <vector>

#include "croiprotocol/croip.h"
#include "croiprotocol/croipreceive.h"
#include "croiprotocol/croippacket.h"

using namespace std;

void threadRunner(Croip::croipReceive *recv) {

    if (recv->init() < 0) {
        return;
    }


    recv->recvLoop();

    recv->deinit();

    return;
}

void usage() {

    std::cout << "usage: "
              <<"./RoombaTCPServer 'port'" << std::endl;
}

int main(int argc,char *argv[])
{
    if(argc != 2) {
        usage();
        return EXIT_FAILURE;
    }

    std::string portS = argv[1];
    int port = std::stoi(portS);

    cout << "Roomba TCP server starting. using port " << port << endl;

    Croip::croipReceive *receive = new Croip::croipReceive(port);

    thread receiver(threadRunner, receive);

    std::vector<Croip::CroipPacket> recvPackets;

    while(true) {
        receive->getPackets(recvPackets);

        if(recvPackets.size() > 0) {
            cout << "received new packets\n";

            for(auto i : recvPackets) {
                char packet[255];
                i.serialize(packet);
                std::cout << "paketti: "<< packet;
            }
        }
        sleep(1);
        cout << "loop..." << endl;
    }

    receiver.join();
    cout << "Roomba TCP server stopping..." << endl;
    return 0;
}

