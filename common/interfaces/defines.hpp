#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

#include <cstddef>
#include <cstdint>
#include <vector>

#define DEFAUILT_TCP_PORT 11111
#define DEFAUILT_UDP_PORT 11112
#define DEFAUILT_GRPC_PORT 11113

typedef uint64_t UserID;
typedef uint32_t Timestamp;

enum DataType : uint8_t { TextMessage, FileMessage };
enum MessageType : uint8_t { UserMessage, ServiceMessage, AckMessage };

struct Header {
    size_t      payload_size;
    size_t      total_size;
    MessageType message_type;
    Timestamp   timestamp;
};

struct EnvelopeMeta {
    Header   header;
    DataType type;
    UserID   sender_id;
    UserID   recepient_id;
};

struct PhoneBookRecord {
    UserID user_id;
    char   nickname[256];
};

struct AuthResponse {
    bool   is_accepted;
    UserID user_id;
};

struct UserCredentials {
    char email[256];
    char password[256];
};

struct SocketInfo {
    sockaddr  addr;
    socklen_t addrlen = sizeof(sockaddr);  // must be defined by size of
                                           // sockaddr by default
};

struct ClientInfo {
    UserID     user_id;
    SocketInfo socket_info;
};

struct Envelope {
    static constexpr size_t size = 1024;

    EnvelopeMeta meta_data;
    char         payload[size - sizeof(EnvelopeMeta)];
};
