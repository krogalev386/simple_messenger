#include "EndpointBase.hpp"

#include "Logger.hpp"
#include "msg_itf.hpp"
#include "SocketPolling.hpp"

#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <vector>
#include <unistd.h>
#include <optional>

EndpointBase::EndpointBase(int port, int type, int protocol, bool blocking)
{
    valid_flag = true;
    if (not blocking)
    {
        type |= SOCK_NONBLOCK;
    }
    socket_id = socket(AF_INET, type, protocol);
    if (socket_id < 0)
    {
        perror("Error: socket initialization failed");
        valid_flag = false;
    }
    address.sin_family = AF_INET;
    address.sin_port   = htons(port);
}

EndpointBase::~EndpointBase()
{
    status = close(socket_id);
    if (status)
    {
        perror("Socket closing is failed");
    }
    else
    {
        printf("connection closed.\n");
    }
}

void EndpointBase::sendEnvelope(const Envelope& envelope, int receiver_handle)
{
    receiver_handle = (receiver_handle == 0) ? socket_id : receiver_handle;
    int64_t n_bytes = send(receiver_handle, &envelope, sizeof(Envelope), 0);
    LOG("%ld bytes has been sent", n_bytes);
}

Envelope EndpointBase::receiveEnvelope(int sender_handle)
{
    sender_handle = (sender_handle == 0) ? socket_id : sender_handle;
    Envelope env;
    memset(&env, 0, sizeof(Envelope));
    int64_t n_bytes = recv(sender_handle, &env, sizeof(Envelope), 0);
    LOG("%ld bytes has been received", n_bytes);
    return env;
}

std::optional<Envelope> EndpointBase::tryReceiveEnvelope(int sender_handle)
{
    const auto [ready_to_read, err_or_closed] = utilities::pollSocket(sender_handle);
    if (ready_to_read)
    {
        return receiveEnvelope(sender_handle);
    }
    else
    {
        return std::nullopt;
    }
}

// This code will be used later
#if 0
std::vector<char> EndpointBase::receiveData(int sender_handle)
{
    // get first block
    memset(buffer, 0, sizeof(buffer));
    int received_data_size = recv(sender_handle, buffer, sizeof(buffer), 0);
    if (received_data_size < 0)
    {
        perror("recv error");
    }
    printf("Message received, parsing...\n");
    auto [payload_size] = parse_header(buffer);
    printf("Size of received message: %d\n", received_data_size);
    printf("Payload size of received message: %ld\n", payload_size);
    std::vector<char> data_payload(payload_size);
    
    // receive data:
    // process first block
    if (payload_size + sizeof(Header) < sizeof(buffer))
    {
        std::memcpy(data_payload.data(), buffer+sizeof(Header), payload_size);
        return std::move(data_payload);
    }

    std::memcpy(data_payload.data(), buffer + sizeof(Header), sizeof(buffer) - sizeof(Header));
    
    // process rest of data
    const size_t total_data_size   = (payload_size + sizeof(Header));

    size_t write_offset = sizeof(buffer) - sizeof(Header);
    size_t read_size;

    while (write_offset < payload_size)
    {
        printf("write_offset: %ld\n", write_offset);
        received_data_size = recv(sender_handle, buffer, sizeof(buffer), 0);
        printf("received_data_size: %d\n", received_data_size);
        std::memcpy(data_payload.data()+write_offset, buffer, received_data_size);
        write_offset += received_data_size;
    }
    return std::move(data_payload);
}

void EndpointBase::sendPackedMessage(const std::vector<char>& packed_message)
{
    size_t n_bytes = send(socket_id, packed_message.data(), packed_message.size()*sizeof(char), 0);
    printf("Sent bytes: %ld\n", n_bytes);
}

void EndpointBase::sendData(const std::vector<char>& message)
{
    std::vector<char> packed_message(message.size() + sizeof(Header));
    Header hdr;
    hdr.payload_size = message.size()*sizeof(char);
    std::memcpy(packed_message.data(), &hdr, sizeof(Header));
    std::memcpy(packed_message.data() + sizeof(Header), message.data(), message.size());
    sendPackedMessage(packed_message);
}
#endif
