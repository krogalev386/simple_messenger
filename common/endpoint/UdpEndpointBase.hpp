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

struct ExpiringAck {
    uint8_t   counter = 3;
    Timestamp tStamp;
};

class UdpEndpointBase : public EndpointBase {
   private:
    static constexpr uint8_t messageQueueSize = 4;

    CircularBuffer<AddressedEnvelope, messageQueueSize> inQueue;
    CircularBuffer<AddressedEnvelope, messageQueueSize> outQueue;
    CircularBuffer<ExpiringAck, messageQueueSize>       ackQueue;

    bool                         isRunning;
    std::unique_ptr<std::thread> sender_thread;
    std::unique_ptr<std::thread> listener_thread;

   public:
    std::mutex ackMtx;

   public:
    UdpEndpointBase(int port, int protocol, bool blocking = true);
    ~UdpEndpointBase();

   public:
    void sendEnvelope(const Envelope&   envelope,
                      const SocketInfo& receiver_info);
    // Envelope receiveEnvelope(const SocketInfo* sender_info = nullptr);

    std::optional<Envelope> tryReceiveEnvelope();

   private:
    void dispatchIncomingEnv(const AddressedEnvelope&);

    void sendEnvFromOutQueue();
    void storeEnvToInQueue(const AddressedEnvelope&);
    void storeTstampToAckQueue(const Timestamp&);

    AddressedEnvelope recvDataGram();
    void              sendDataGram(const AddressedEnvelope&);

    void repeatingSend();
    void repeatingListen();

    bool checkAckQueue(const Timestamp&);
};
