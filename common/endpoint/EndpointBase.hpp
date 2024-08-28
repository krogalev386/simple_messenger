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

struct SocketInfo
{
    sockaddr addr;
    socklen_t addrlen;
};

struct ClientInfo
{
    int handle;
    SocketInfo socket_info;
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
    void     sendEnvelope(const Envelope& envelope, int receiver_handle = 0);
    Envelope receiveEnvelope(int sender_handle = 0);

    std::optional<Envelope> tryReceiveEnvelope(int sender_handle);
    sockaddr_in getAddress() { return address;}

protected:
    std::tuple<bool, bool> pollSocket(int socket_id);

protected:
    sockaddr_in address;

    int  socket_id;
    int  status;
    bool valid_flag;
    char buffer[buff_size];
};
