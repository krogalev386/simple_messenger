#pragma once

#include <optional>

#include "EndpointBase.hpp"
#include "defines.hpp"

class UdpEndpointBase : public EndpointBase {
   public:
    UdpEndpointBase(int port, int protocol, bool blocking = true)
        : EndpointBase(port, SOCK_DGRAM, protocol, blocking){};

   public:
    void     sendEnvelope(const Envelope&   envelope,
                          const SocketInfo& receiver_info);
    Envelope receiveEnvelope(const SocketInfo* sender_info = nullptr);

    std::optional<Envelope> tryReceiveEnvelope(
        const SocketInfo* sender_info = nullptr);
};
