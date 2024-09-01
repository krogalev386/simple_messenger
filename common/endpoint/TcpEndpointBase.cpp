#include "TcpEndpointBase.hpp"

#include "Logger.hpp"

#include <cstring>


void TcpEndpointBase::sendEnvelope(const Envelope& envelope, int receiver_handle)
{
    receiver_handle = (receiver_handle == 0) ? socket_id : receiver_handle;
    int64_t n_bytes = send(receiver_handle, &envelope, sizeof(Envelope), 0);
    LOG("%ld bytes has been sent", n_bytes);
}

Envelope TcpEndpointBase::receiveEnvelope(int sender_handle)
{
    sender_handle = (sender_handle == 0) ? socket_id : sender_handle;
    Envelope env;
    memset(&env, 0, sizeof(Envelope));
    int64_t n_bytes = recv(sender_handle, &env, sizeof(Envelope), 0);
    LOG("%ld bytes has been received", n_bytes);
    return env;
}

std::optional<Envelope> TcpEndpointBase::tryReceiveEnvelope(int sender_handle)
{
    const auto [ready_to_read, err_or_closed] = pollSocket(sender_handle);
    if (ready_to_read)
    {
        return receiveEnvelope(sender_handle);
    }
    else
    {
        return std::nullopt;
    }
}
