#include "UdpEndpointBase.hpp"

#include <cstring>

#include "Logger.hpp"

void UdpEndpointBase::sendEnvelope(const Envelope&   envelope,
                                   const SocketInfo& receiver_info) {
    int64_t n_bytes = sendto(socket_id, &envelope, sizeof(Envelope), 0,
                             const_cast<sockaddr*>(&receiver_info.addr),
                             receiver_info.addrlen);
    LOG("%ld bytes has been sent over UDP", n_bytes);
}

Envelope UdpEndpointBase::receiveEnvelope(const SocketInfo* sender_info) {
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
    LOG("%ld bytes has been received over UDP", n_bytes);
    return env;
}

std::optional<Envelope> UdpEndpointBase::tryReceiveEnvelope(
    const SocketInfo* sender_info) {
    const auto [ready_to_read, err_or_closed] = pollSocket(socket_id);
    if (ready_to_read) {
        return receiveEnvelope(sender_info);
    }
    return std::nullopt;
}
