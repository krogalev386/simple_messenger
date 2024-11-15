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

void UdpEndpointBase::sendEnvelope(const Envelope&   envelope,
                                   const SocketInfo& receiver_info) {
    int64_t n_bytes = sendto(socket_id, &envelope, sizeof(Envelope), 0,
                             const_cast<sockaddr*>(&receiver_info.addr),
                             receiver_info.addrlen);
    LOG("%ld bytes has been sent over UDP", n_bytes);
}

void UdpEndpointBase::sendAck(const SocketInfo& receiver_info,
                              Timestamp         t_stamp) {
    Envelope env{};
    memset(&env, 0, sizeof(Envelope));
    msg_proc::setMessageType(env, MessageType::AckMessage);
    msg_proc::setTimeStamp(env, t_stamp);
    int64_t n_bytes = sendto(socket_id, &env, sizeof(Envelope), 0,
                             const_cast<sockaddr*>(&receiver_info.addr),
                             receiver_info.addrlen);
    LOG("%ld ACK message has been sent over UDP", n_bytes);
}

bool UdpEndpointBase::sendEnvelopeAck(const Envelope&   envelope,
                                      const SocketInfo& receiver_info) {
    std::unique_lock<std::mutex> lock(ackMtx);
    // Timestamp envelope before to send
    timeval                      curTime{};
    gettimeofday(&curTime, NULL);
    Timestamp tStamp = (static_cast<uint32_t>(curTime.tv_sec) << 20) |
                       (static_cast<uint32_t>(curTime.tv_usec));

    Envelope envStamped = envelope;
    msg_proc::setTimeStamp(envStamped, tStamp);

    for (uint8_t i = 0; i < 10; i++) {
        sendEnvelope(envStamped, receiver_info);
        std::this_thread::sleep_for(std::chrono::milliseconds(50 * (i + 1)));
        auto response = tryReceiveAck(&receiver_info);
        if ((response) and
            (msg_proc::getMessageType(*response) == MessageType::AckMessage) and
            (msg_proc::getTimeStamp(*response) == tStamp)) {
            return true;
        }
    }
    LOG("WRN: Transmission failed.");
    return false;
}

Envelope recvEnv(const int socket_id, const SocketInfo* sender_info) {
    Envelope env{};
    memset(&env, 0, sizeof(Envelope));
    int64_t n_bytes = 0;
    if (sender_info != nullptr) {
        n_bytes = recvfrom(socket_id, &env, sizeof(Envelope), 0,
                           const_cast<sockaddr*>(&(sender_info->addr)),
                           const_cast<socklen_t*>(&(sender_info->addrlen)));
    } else {
        n_bytes = recv(socket_id, &env, sizeof(Envelope), 0);
    }
    return env;
}

Envelope UdpEndpointBase::receiveEnvelope(const SocketInfo* sender_info) {
    Envelope env = recvEnv(socket_id, sender_info);
    if (msg_proc::getMessageType(env) != MessageType::AckMessage) {
        if (sender_info != nullptr) {
            // if sender is known - send ACK message back
            sendAck(*sender_info, msg_proc::getTimeStamp(env));
        }
    } else {
        LOG("ERROR: ACK message catched by non-ACK receive handler!");
    }
    return env;
}

Envelope UdpEndpointBase::receiveAck(const SocketInfo* sender_info) {
    Envelope env = recvEnv(socket_id, sender_info);
    if (msg_proc::getMessageType(env) != MessageType::AckMessage) {
        LOG("ERROR: Non-ACK message catched by ACK receive handler!");
    }
    return env;
}

std::optional<Envelope> UdpEndpointBase::tryReceiveAck(
    const SocketInfo* sender_info) {
    const auto [ready_to_read, err_or_closed] = pollSocket(socket_id);
    if (ready_to_read) {
        return receiveAck(sender_info);
    }
    return std::nullopt;
}

std::optional<Envelope> UdpEndpointBase::tryReceiveEnvelope(
    const SocketInfo* sender_info) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    {
        std::unique_lock<std::mutex> lock(ackMtx);
        const auto [ready_to_read, err_or_closed] = pollSocket(socket_id);
        if (ready_to_read) {
            return receiveEnvelope(sender_info);
        }
        return std::nullopt;
    }
}
