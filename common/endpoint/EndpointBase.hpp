#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <memory>
#include <optional>
#include <tuple>
#include <vector>

#include "defines.hpp"
#include "proto_defines.pb.h"

class EndpointBase {
   public:
    EndpointBase(int port, int type, int protocol, bool blocking = true);
    ~EndpointBase();

    bool is_valid() { return valid_flag; }

    sockaddr_in getAddress() { return address; }

    static std::tuple<bool, bool> pollSocket(int socket_id);

   protected:
    void                    putEnvToProtoBufferOut(const Envelope& raw_env);
    std::optional<Envelope> getEnvFromProtoBufferIn();

   protected:
    struct RxTxBuffer {
        size_t message_size;
        char   payload[sizeof(Envelope)];
    };
    sockaddr_in address{};

    int                    socket_id;
    int                    status{};
    bool                   valid_flag;
    protobuf_itf::Envelope proto_message_buffer_in;
    protobuf_itf::Envelope proto_message_buffer_out;
    RxTxBuffer             buffer_in{};
    RxTxBuffer             buffer_out{};
};
