#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

typedef uint64_t UserID;

enum DataType : uint8_t
{
    TextMessage,
    FileMessage
};

enum MessageType : uint8_t
{
    UserMessage,
    ServiceMessage
};

struct Header
{
    size_t      payload_size;
    size_t      total_size;
    MessageType message_type;
};

struct EnvelopeMeta
{
    Header            header;
    DataType          type;
    UserID            sender_id;
    UserID            recipient_id;
};

struct AbonentBaseRecord
{
    UserID user_id;
    char   nickname[256];
    char   password[256];
};

struct Envelope
{
    static constexpr size_t size = 1024;

    EnvelopeMeta      meta_data;
    char              payload[size - sizeof(EnvelopeMeta)];
};
