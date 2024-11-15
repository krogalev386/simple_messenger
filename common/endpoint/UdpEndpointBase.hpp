#pragma once

#include <mutex>
#include <optional>

#include "EndpointBase.hpp"
#include "defines.hpp"

class UdpEndpointBase : public EndpointBase {
   private:
    std::optional<Envelope> ackEnvelope;

   public:
    std::mutex ackMtx;

   public:
    UdpEndpointBase(int port, int protocol, bool blocking = true)
        : EndpointBase(port, SOCK_DGRAM, protocol, blocking),
          ackEnvelope(std::nullopt){};

   public:
    void     sendEnvelope(const Envelope&   envelope,
                          const SocketInfo& receiver_info);
    void     sendAck(const SocketInfo& receiver_info, Timestamp t_stamp);
    bool     sendEnvelopeAck(const Envelope&   envelope,
                             const SocketInfo& receiver_info);
    Envelope receiveEnvelope(const SocketInfo* sender_info = nullptr);

    void updateAckEnvelope(const Envelope& ackEnv) {
        ackEnvelope = std::make_optional<Envelope>(ackEnv);
    };
    std::optional<Envelope> tryReceiveEnvelope(
        const SocketInfo* sender_info = nullptr);

   private:
    Envelope                receiveAck(const SocketInfo* sender_info = nullptr);
    std::optional<Envelope> tryReceiveAck(
        const SocketInfo* sender_info = nullptr);
};
