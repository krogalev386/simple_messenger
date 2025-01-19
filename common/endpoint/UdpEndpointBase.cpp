#include "UdpEndpointBase.hpp"

#include <sys/time.h>

#include <chrono>
#include <cstring>
#include <mutex>
#include <thread>

#include "Logger.hpp"
#include "MessageProcessing.hpp"
#include "ThreadManager.hpp"
#include "defines.hpp"

UdpEndpointBase::UdpEndpointBase(int port, int protocol, bool blocking)
    : EndpointBase(port, SOCK_DGRAM, protocol, blocking), isRunning(true) {
    sender_thread =
        std::make_unique<std::thread>(&UdpEndpointBase::repeatingSend, this);
    listener_thread =
        std::make_unique<std::thread>(&UdpEndpointBase::repeatingListen, this);
};

UdpEndpointBase::~UdpEndpointBase() {
    isRunning = false;
    sender_thread->join();
    listener_thread->join();
}

void UdpEndpointBase::sendEnvelope(const Envelope&   envelope,
                                   const SocketInfo& receiver_info) {
    Envelope stampedEnv = envelope;
    msg_proc::setTimeStamp(stampedEnv, msg_proc::createTimeStamp());
    AddressedEnvelope addrEnv{receiver_info, stampedEnv};

    bool success = outQueue.push(addrEnv);
    if (success) {
        LOG("Outcoming message enqueued for departure");
    }
}

AddressedEnvelope UdpEndpointBase::recvDataGram() {
    Envelope env{};
    memset(&env, 0, sizeof(Envelope));
    SocketInfo sender_info;
    memset(&sender_info.addr, 0, sizeof(sockaddr));

    // sender_info should be filled by sender address after the reception
    int64_t n_bytes =
        recvfrom(socket_id, &(this->buffer_in), sizeof(this->buffer_in), 0,
                 &(sender_info.addr), &(sender_info.addrlen));
    std::optional<Envelope> res = getEnvFromProtoBuffer(Direction::IN);
    if (res) {
        env = res.value();
    }
    LOG("%ld bytes received from and enqueued", n_bytes);

    AddressedEnvelope aEnv{sender_info, env};
    return aEnv;
}

void UdpEndpointBase::storeEnvToInQueue(const AddressedEnvelope& aEnv) {
    while (not inQueue.push(aEnv)) {
        inQueue.pop();
        LOG("WRN: incoming message queue overflow, earliest message is "
            "dropped");
    }
}

void UdpEndpointBase::storeTstampToAckQueue(const Timestamp& tStamp) {
    ExpiringAck eAck;
    eAck.tStamp = tStamp;
    while (not ackQueue.push(eAck)) {
        ackQueue.pop();
        LOG("WRN: ACK queue overflow, earliest ACK is dropped");
    }
}

void UdpEndpointBase::sendDataGram(const AddressedEnvelope& aEnv) {
    putEnvToProtoBuffer(aEnv.env, Direction::OUT);
    int64_t n_bytes = sendto(
        socket_id, &this->buffer_out, sizeof(this->buffer_out), 0,
        const_cast<sockaddr*>(&aEnv.sock_info.addr), aEnv.sock_info.addrlen);
    LOG("%ld bytes has been sent over UDP", n_bytes);
    if (n_bytes < 0) {
        perror("send failure");
    }
}

void UdpEndpointBase::sendEnvFromOutQueue() {
    auto addrEnvOpt = outQueue.pop();
    if (not addrEnvOpt) {
        return;
    }

    Timestamp tStamp = msg_proc::getTimeStamp(addrEnvOpt->env);

    for (uint8_t attempt = 0; attempt < 5; attempt++) {
        sendDataGram(*addrEnvOpt);
        // waiting for ACK
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100 * (attempt + 1)));
        if (checkAckQueue(tStamp)) {
            return;
        }
    }
}

bool UdpEndpointBase::checkAckQueue(const Timestamp& tStamp) {
    auto pendingAck = ackQueue.pop();
    if (not pendingAck) {
        return false;
    }
    LOG("pendingAck->tStamp: %lu, tStamp: %lu", pendingAck->tStamp, tStamp);
    if (pendingAck->tStamp == tStamp) {
        LOG("ACK received!");
        return true;
    }
    pendingAck->counter--;
    if (pendingAck->counter > 0) {
        ackQueue.push(*pendingAck);
    }
    return false;
}

void UdpEndpointBase::repeatingSend() {
    while (isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        sendEnvFromOutQueue();
    }
}

void UdpEndpointBase::dispatchIncomingEnv(const AddressedEnvelope& aEnv) {
    if (msg_proc::getMessageType(aEnv.env) == MessageType::AckMessage) {
        storeTstampToAckQueue(msg_proc::getTimeStamp(aEnv.env));
    } else {
        storeEnvToInQueue(aEnv);
    }
}

void UdpEndpointBase::repeatingListen() {
    while (isRunning) {
        const auto [ready_to_read, err_or_closed] = pollSocket(socket_id);
        if (ready_to_read) {
            auto env = recvDataGram();
            dispatchIncomingEnv(env);
        }
    }
}

std::optional<Envelope> UdpEndpointBase::tryReceiveEnvelope() {
    auto aEnv = inQueue.pop();
    if (not aEnv) {
        return std::nullopt;
    }
    LOG("message retrieved from incoming messages queue");
    // Send ACK notification
    Envelope ackEnv{};
    msg_proc::setMessageType(ackEnv, MessageType::AckMessage);
    msg_proc::setTimeStamp(ackEnv, msg_proc::getTimeStamp(aEnv->env));

    AddressedEnvelope ackNotification{aEnv->sock_info, ackEnv};
    sendDataGram(ackNotification);

    return aEnv->env;
}
