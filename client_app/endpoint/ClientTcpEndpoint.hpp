#pragma once

#include "TcpEndpointBase.hpp"

class ClientTcpEndpoint : public TcpEndpointBase {
   public:
    ClientTcpEndpoint(int port) : TcpEndpointBase(port, 0){};

    void connectTo(const char* ip_address, int server_port);
};
