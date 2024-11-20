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
    AddressedEnvelope addrEnv{receiver_info, envelope};
    bool              success = outQueue.push(addrEnv);
    if (success) {
        LOG("Outcoming message enqueued for departure");
    }
}

Envelope UdpEndpointBase::recvEnv(const SocketInfo* sender_info) {
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
    LOG("%ld bytes received and enqueued", n_bytes);
    return env;
}

void UdpEndpointBase::storeEnvToInQueue(const Envelope& env) {
    bool success = inQueue.push(env);
    if (not success) {
        LOG("ERROR: incoming message queue overflowed, message dropped");
    }
}

void UdpEndpointBase::sendEnvFromOutQueue() {
    auto addrEnvOpt = outQueue.pop();
    if (addrEnvOpt) {
        Envelope   envelope      = addrEnvOpt->env;
        SocketInfo receiver_info = addrEnvOpt->sock_info;
        int64_t    n_bytes = sendto(socket_id, &envelope, sizeof(Envelope), 0,
                                    const_cast<sockaddr*>(&receiver_info.addr),
                                    receiver_info.addrlen);
        LOG("%ld bytes has been sent over UDP", n_bytes);
        if (n_bytes < 0) {
            perror("send failure");
        }
    }
}

void UdpEndpointBase::repeatingSend() {
    while (isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        sendEnvFromOutQueue();
    }
}

void UdpEndpointBase::repeatingListen() {
    while (isRunning) {
        const auto [ready_to_read, err_or_closed] = pollSocket(socket_id);
        if (ready_to_read) {
            Envelope env = recvEnv(nullptr);
            storeEnvToInQueue(env);
        }
    }
}

std::optional<Envelope> UdpEndpointBase::tryReceiveEnvelope(
    const SocketInfo* sender_info) {
    if (sender_info != nullptr) {
        LOG("WARNING: functionality currently not supported, 'sender_info' "
            "will be ignored");
    }
    auto env = inQueue.pop();
    if (env) {
        return env;
    }
    return std::nullopt;
}
