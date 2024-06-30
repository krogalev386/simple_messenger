#pragma once

#include "EndpointBase.hpp"

class ClientEndpoint : public EndpointBase
{
public:
    ClientEndpoint(int port, int type) : EndpointBase(port, type, 0) {};

    void connectTo(const char* ip_address, int server_port);
};
