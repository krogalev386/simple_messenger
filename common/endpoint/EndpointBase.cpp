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
