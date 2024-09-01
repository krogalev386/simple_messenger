#pragma once

#include "UdpEndpointBase.hpp"

class ClientUdpEndpoint : public UdpEndpointBase
{
public:
    ClientUdpEndpoint(int port) : UdpEndpointBase(port, 0) {};
};
