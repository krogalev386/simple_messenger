#pragma once

#include "UdpEndpointBase.hpp"

class ClientUdpEndpoint : public UdpEndpointBase {
   public:
    ClientUdpEndpoint(int port) : UdpEndpointBase(port, 0) {
        address.sin_addr.s_addr = INADDR_ANY;
        status = bind(socket_id, reinterpret_cast<sockaddr*>(&address),
                      sizeof(address));
        if (status != 0) {
            perror("Error: socket binding failed");
        }
    };
};
