#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <optional>
#include <tuple>
#include <vector>

#include "defines.hpp"

constexpr size_t buff_size = sizeof(Envelope);

class EndpointBase {
   public:
    EndpointBase(int port, int type, int protocol, bool blocking = true);
    ~EndpointBase();

    bool is_valid() { return valid_flag; }

    sockaddr_in getAddress() { return address; }

    static std::tuple<bool, bool> pollSocket(int socket_id);

   protected:
   protected:
    sockaddr_in address{};

    int  socket_id;
    int  status{};
    bool valid_flag;
    char buffer[buff_size]{};
};
