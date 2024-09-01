#include "SocketPolling.hpp"

#include "Logger.hpp"

#include <poll.h>

namespace utilities
{
std::tuple<bool, bool> pollSocket(int socket_id)
{
    bool ready_to_hanlde = false;
    bool err_or_closed   = false;

    pollfd p_fd;
    p_fd.fd = socket_id;
    p_fd.events = POLLIN;
    int poll_res = poll(&p_fd, 1, 100);

    if (poll_res == -1)
    {
        LOG("ERROR: socket pooling failed");
        perror("ERROR: socket pooling failed");
        err_or_closed = true;
        return {ready_to_hanlde, err_or_closed};
    }

    err_or_closed = ((p_fd.revents & POLLHUP) | (p_fd.revents & POLLERR)) ? true : false;
    if (err_or_closed)
    {
        LOG("Connection closed");
        return {ready_to_hanlde, err_or_closed};
    }

    ready_to_hanlde = ((p_fd.revents & POLLIN) | (p_fd.revents & POLLPRI)) ? true : false;
    if (ready_to_hanlde)
    {
        LOG("ready_to_hanlde = %d, p_fd.revents = %d", ready_to_hanlde, p_fd.revents);
    }
    return {ready_to_hanlde, err_or_closed};
};
};

