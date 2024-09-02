#include "ServerUdpEndpoint.hpp"
#include <cstdio>

ServerUdpEndpoint::ServerUdpEndpoint(int port, bool blocking) : UdpEndpointBase(port, 0, blocking)
{
    address.sin_addr.s_addr = INADDR_ANY;
    status = bind(socket_id, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    if (status != 0)
    {
        perror("Error: socket binding failed");
    }
};
