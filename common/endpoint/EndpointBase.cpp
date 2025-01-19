#include "EndpointBase.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <optional>
#include <vector>

#include "Logger.hpp"
#include "MessageProcessing.hpp"

EndpointBase::EndpointBase(int port, int type, int protocol, bool blocking)
    : valid_flag(true) {
    if (not blocking) {
        type |= SOCK_NONBLOCK;
    }
    socket_id = socket(AF_INET, type, protocol);
    if (socket_id < 0) {
        perror("Error: socket initialization failed");
        valid_flag = false;
    }
    address.sin_family = AF_INET;
    address.sin_port   = htons(port);
}

EndpointBase::~EndpointBase() {
    status = close(socket_id);
    if (status != 0) {
        perror("Socket closing is failed");
    } else {
        printf("connection closed.\n");
    }
}

std::tuple<bool, bool> EndpointBase::pollSocket(int socket_id) {
    bool ready_to_hanlde = false;
    bool err_or_closed   = false;

    pollfd p_fd{};
    p_fd.fd      = socket_id;
    p_fd.events  = POLLIN;
    int poll_res = poll(&p_fd, 1, 100);

    if (poll_res == -1) {
        LOG("ERROR: socket pooling failed");
        perror("ERROR: socket pooling failed");
        err_or_closed = true;
        return {ready_to_hanlde, err_or_closed};
    }

    err_or_closed = ((p_fd.revents & POLLHUP) | (p_fd.revents & POLLERR)) != 0;
    if (err_or_closed) {
        LOG("Connection closed");
        return {ready_to_hanlde, err_or_closed};
    }

    ready_to_hanlde = ((p_fd.revents & POLLIN) | (p_fd.revents & POLLPRI)) != 0;
    if (ready_to_hanlde) {
        LOG("ready_to_hanlde = %d, p_fd.revents = %d", ready_to_hanlde,
            p_fd.revents);
    }
    return {ready_to_hanlde, err_or_closed};
};

void EndpointBase::putEnvToProtoBuffer(const Envelope& raw_env,
                                       const Direction dir) {
    protobuf_itf::Envelope* proto_env = nullptr;
    RxTxBuffer*             buffer = nullptr;
    if (dir == Direction::IN) {
        proto_env = &proto_message_buffer_in;
        buffer    = &buffer_in;
    } else if (dir == Direction::OUT) {
        proto_env = &proto_message_buffer_out;
        buffer    = &buffer_out;
    } else {
        LOG("ERROR: direction of message is not determined");
        return;
    }

    protobuf_itf::EnvelopeMeta* proto_meta_data =
        proto_env->mutable_meta_data();
    protobuf_itf::Header* proto_header = proto_meta_data->mutable_header();

    switch (msg_proc::getMessageType(raw_env)) {
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

    proto_header->set_timestamp(msg_proc::getTimeStamp(raw_env));

    switch (raw_env.meta_data.type) {
        case DataType::TextMessage:
            proto_meta_data->set_type(protobuf_itf::TextMessage);
            break;
        case DataType::FileMessage:
            proto_meta_data->set_type(protobuf_itf::FileMessage);
            break;
    }

    proto_meta_data->set_sender_id(msg_proc::getSenderID(raw_env));
    proto_meta_data->set_recepient_id(msg_proc::getRecepientID(raw_env));

    proto_env->set_payload(raw_env.payload);
    LOG("Protobuf message size is %d bytes", proto_env->ByteSizeLong());

    if (proto_env->ByteSizeLong() > Envelope::size) {
        LOG("ERROR: message is bigger than available space in transmission "
            "buffer");
        return;
    }
    buffer->message_size = proto_env->ByteSizeLong();
    // memcpy(buffer->payload, proto_env, proto_env->ByteSizeLong());
    proto_env->SerializeToArray(buffer->payload, buffer->message_size);
};

std::optional<Envelope> EndpointBase::getEnvFromProtoBuffer(
    const Direction dir) {
    protobuf_itf::Envelope* proto_env = nullptr;
    RxTxBuffer*             buffer = nullptr;
    if (dir == Direction::IN) {
        proto_env = &proto_message_buffer_in;
        buffer    = &buffer_in;
    } else if (dir == Direction::OUT) {
        proto_env = &proto_message_buffer_out;
        buffer    = &buffer_out;
    } else {
        LOG("ERROR: direction of message is not determined");
        return std::nullopt;
    }

    proto_env->ParseFromArray(buffer->payload, buffer->message_size);

    LOG("Extracted message size is %d bytes", proto_env->ByteSizeLong());

    protobuf_itf::EnvelopeMeta* proto_meta_data =
        proto_env->mutable_meta_data();
    protobuf_itf::Header* proto_header = proto_meta_data->mutable_header();

    Envelope raw_env{};

    switch (proto_header->message_type()) {
        case protobuf_itf::UserMessage:
            msg_proc::setMessageType(raw_env, MessageType::UserMessage);
            break;
        case protobuf_itf::ServiceMessage:
            msg_proc::setMessageType(raw_env, MessageType::ServiceMessage);
            break;
        case protobuf_itf::AckMessage:
            msg_proc::setMessageType(raw_env, MessageType::AckMessage);
            break;
    }

    msg_proc::setTimeStamp(raw_env, proto_header->timestamp());

    switch (proto_meta_data->type()) {
        case protobuf_itf::TextMessage:
            raw_env.meta_data.type = DataType::TextMessage;
            break;
        case protobuf_itf::FileMessage:
            raw_env.meta_data.type = DataType::FileMessage;
            break;
    }

    msg_proc::setSenderId(raw_env, proto_meta_data->sender_id());
    msg_proc::setRecepientId(raw_env, proto_meta_data->recepient_id());

    LOG("Extracted payload size is %d bytes", proto_env->payload().size());
    memset(&raw_env.payload, 0, sizeof(raw_env.payload));
    const char* c_payload = (proto_env->payload()).c_str();
    memcpy(&raw_env.payload, c_payload, std::strlen(c_payload));
    return raw_env;
};
