#pragma once

#include <CircularBuffer.hpp>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>

#include "EndpointBase.hpp"
#include "defines.hpp"

struct AddressedEnvelope {
    SocketInfo sock_info;
    Envelope   env;
};

class UdpEndpointBase : public EndpointBase {
   private:
    static constexpr uint8_t messageQueueSize = 4;

    // std::optional<Envelope> ackEnvelope;

    CircularBuffer<Envelope, messageQueueSize>          inQueue;
    CircularBuffer<AddressedEnvelope, messageQueueSize> outQueue;
    bool                                                isRunning;
    std::unique_ptr<std::thread>                        sender_thread;
    std::unique_ptr<std::thread>                        listener_thread;

   public:
    std::mutex ackMtx;

   public:
    UdpEndpointBase(int port, int protocol, bool blocking = true);
    ~UdpEndpointBase();

   public:
    void sendEnvelope(const Envelope&   envelope,
                      const SocketInfo& receiver_info);
    // Envelope receiveEnvelope(const SocketInfo* sender_info = nullptr);

    std::optional<Envelope> tryReceiveEnvelope(
        const SocketInfo* sender_info = nullptr);

   private:
    void     sendEnvFromOutQueue();
    void     storeEnvToInQueue(const Envelope& env);
    Envelope recvEnv(const SocketInfo* sender_info);
    void     repeatingSend();
    void     repeatingListen();
};
