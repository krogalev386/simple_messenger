#pragma once

#include <chrono>
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

inline UserID getRecepientID(const Envelope& env) {
    return env.meta_data.recipient_id;
};
};  // namespace msg_proc
