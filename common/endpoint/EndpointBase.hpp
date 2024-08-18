#pragma once

#include "defines.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <arpa/inet.h>
#include <unistd.h>
#include <optional>
#include <tuple>

constexpr size_t buff_size = sizeof(Envelope);

struct ClientInfo
{
    int handle;
    sockaddr addr;
    socklen_t addrlen;
};

class EndpointBase
{
public:
    EndpointBase(int port, int type, int protocol, bool blocking = true);
    ~EndpointBase();

    bool is_valid() {return valid_flag;}

// This code will be used later
#if 0
    std::vector<char> receiveData(int sender_handle);

    void sendPackedMessage(const std::vector<char>& packed_message);
    void sendData(const std::vector<char>& message);
#endif
    void     sendEnvelope(const Envelope& envelope);
    Envelope receiveEnvelope(int sender_handle);

    std::optional<Envelope> tryReceiveEnvelope(int sender_handle);

protected:
    std::tuple<bool, bool> pollSocket(int socket_id);

protected:
    sockaddr_in address;

    int  socket_id;
    int  status;
    bool valid_flag;
    char buffer[buff_size];
};
