#pragma once

#include <vector>

#include "defines.hpp"

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

};  // namespace msg_proc
