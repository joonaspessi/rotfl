#ifndef IROOMBA_H
#define IROOMBA_H

#include <exception>

namespace Croi {

class IRoomba
{

public:
    virtual ~IRoomba() {}

    virtual int connect() = 0;
    //virtual int disconnect() = 0;
    //virtual bool isConnected = 0;

    virtual void Drive( int Velocity, int Radius ) = 0;

};


} //namespace CROI


#endif // IROOMBA_H
