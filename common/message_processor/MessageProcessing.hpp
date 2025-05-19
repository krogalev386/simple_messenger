#pragma once

#include <chrono>
#include <vector>

#include "defines.hpp"
#include "proto_defines.pb.h"

#include "Logger.hpp"

namespace msg_proc {
template <typename T>
T get_payload(const Envelope& env) {
    T typed_payload;
    memcpy(&typed_payload, &env.payload, sizeof(T));
    return typed_payload;
};

template <typename T>
void set_payload(Envelope& env, const T& message) {
    memcpy(&env.payload, &message, sizeof(T));
};

inline MessageType getMessageType(const Envelope& env) {
    return env.meta_data.header.message_type;
};

inline void setMessageType(Envelope& env, MessageType type) {
    env.meta_data.header.message_type = type;
};

inline Timestamp getTimeStamp(const Envelope& env) {
    return env.meta_data.header.timestamp;
};

inline void setTimeStamp(Envelope& env, Timestamp t_stamp) {
    env.meta_data.header.timestamp = t_stamp;
};

inline Timestamp createTimeStamp() {
    using namespace std::chrono;
    const auto now_tse    = system_clock::now().time_since_epoch();
    Timestamp  now_tse_us = duration_cast<microseconds>(now_tse).count();
    return now_tse_us;
};

inline UserID getSenderID(const Envelope& env) {
    return env.meta_data.sender_id;
};

inline void setSenderId(Envelope& env, UserID id) {
    env.meta_data.sender_id = id;
};

inline UserID getRecepientID(const Envelope& env) {
    return env.meta_data.recepient_id;
};

inline void setRecepientId(Envelope& env, UserID id) {
    env.meta_data.recepient_id = id;
};

inline void putEnvToProtoBuf(const Envelope&         raw_env,
                             protobuf_itf::Envelope& proto_env) {
    protobuf_itf::EnvelopeMeta* proto_meta_data = proto_env.mutable_meta_data();
    protobuf_itf::Header* proto_header = proto_meta_data->mutable_header();

    switch (getMessageType(raw_env)) {
        case MessageType::UserMessage:
            proto_header->set_message_type(protobuf_itf::UserMessage);
            break;
        case MessageType::ServiceMessage:
            proto_header->set_message_type(protobuf_itf::ServiceMessage);
            break;
        case MessageType::AckMessage:
            proto_header->set_message_type(protobuf_itf::AckMessage);
            break;
    }

    proto_header->set_timestamp(getTimeStamp(raw_env));

    switch (raw_env.meta_data.type) {
        case DataType::TextMessage:
            proto_meta_data->set_type(protobuf_itf::TextMessage);
            break;
        case DataType::FileMessage:
            proto_meta_data->set_type(protobuf_itf::FileMessage);
            break;
    }

    proto_meta_data->set_sender_id(getSenderID(raw_env));
    proto_meta_data->set_recepient_id(getRecepientID(raw_env));

    proto_env.set_payload(raw_env.payload);
    LOG("Protobuf message size is %d bytes", proto_env.ByteSizeLong());

    if (proto_env.ByteSizeLong() > Envelope::size) {
        LOG("ERROR: message is bigger than available space in transmission "
            "buffer");
        return;
    }
};

inline Envelope getEnvFromProtoBuf(const protobuf_itf::Envelope& proto_env) {
    protobuf_itf::EnvelopeMeta proto_meta_data = proto_env.meta_data();
    protobuf_itf::Header       proto_header    = proto_meta_data.header();

    Envelope raw_env{};

    switch (proto_header.message_type()) {
        case protobuf_itf::UserMessage:
            setMessageType(raw_env, MessageType::UserMessage);
            break;
        case protobuf_itf::ServiceMessage:
            setMessageType(raw_env, MessageType::ServiceMessage);
            break;
        case protobuf_itf::AckMessage:
            setMessageType(raw_env, MessageType::AckMessage);
            break;
    }

    setTimeStamp(raw_env, proto_header.timestamp());

    switch (proto_meta_data.type()) {
        case protobuf_itf::TextMessage:
            raw_env.meta_data.type = DataType::TextMessage;
            break;
        case protobuf_itf::FileMessage:
            raw_env.meta_data.type = DataType::FileMessage;
            break;
    }

    setSenderId(raw_env, proto_meta_data.sender_id());
    setRecepientId(raw_env, proto_meta_data.recepient_id());

    LOG("Extracted payload size is %d bytes", proto_env.payload().size());
    memset(&raw_env.payload, 0, sizeof(raw_env.payload));
    const char* c_payload = (proto_env.payload()).c_str();
    memcpy(&raw_env.payload, c_payload, std::strlen(c_payload));
    return raw_env;
};
};  // namespace msg_proc
