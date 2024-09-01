#include "ServerUdpEndpoint.hpp"
#include <stdio.h>

ServerUdpEndpoint::ServerUdpEndpoint(int port, bool blocking) : UdpEndpointBase(port, 0, blocking)
{
    address.sin_addr.s_addr = INADDR_ANY;
    status = bind(socket_id, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    if (status)
    {
        perror("Error: socket binding failed");
    }
};
