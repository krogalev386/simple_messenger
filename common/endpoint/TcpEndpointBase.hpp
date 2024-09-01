#pragma once

#include "EndpointBase.hpp"

#include "defines.hpp"

#include <optional>

class TcpEndpointBase : public EndpointBase
{
public:
    TcpEndpointBase(int port, int protocol, bool blocking = true) : 
        EndpointBase(port, SOCK_STREAM, protocol, blocking) {};
    
public:
    void     sendEnvelope(const Envelope& envelope, int receiver_handle = 0);
    Envelope receiveEnvelope(int sender_handle = 0);

    std::optional<Envelope> tryReceiveEnvelope(int sender_handle);
};
