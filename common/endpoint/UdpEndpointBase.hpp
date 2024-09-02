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
    Envelope receiveEnvelope();
    Envelope receiveEnvelopeFrom(const SocketInfo& sender_info);

    std::optional<Envelope> tryReceiveEnvelope();
    std::optional<Envelope> tryReceiveEnvelopeFrom(
        const SocketInfo& sender_info);
};
