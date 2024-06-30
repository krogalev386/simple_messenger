#include "ServerEndpoint.hpp"

#include "Logger.hpp"

#include <arpa/inet.h>
#include <cstdio>
#include <optional>
#include <poll.h>
#include <netinet/in.h>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>

ServerEndpoint::ServerEndpoint(int port, int type, bool blocking) : EndpointBase(port, type, 0, blocking)
{
    address.sin_addr.s_addr = INADDR_ANY;
    status = bind(socket_id, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    if (status)
    {
        perror("Error: socket binding failed");
    }
};

void ServerEndpoint::listenConnections(int queue_size)
{
    status = listen(socket_id, queue_size);
    if (status)
    {
        perror("Error: socket listening fails");
    }
};

int ServerEndpoint::acceptConnection()
{
    ClientInfo new_client;
    int client_socket_id = accept(socket_id, &new_client.addr, &new_client.addrlen);
    LOG("client socket id: %d", client_socket_id);
    if (client_socket_id < 0)
    {
        perror("Error: acception failed");
    }
    new_client.handle = client_socket_id;
    client_info_storage.push_back(new_client);
    return client_socket_id;
};

std::optional<int> ServerEndpoint::tryAcceptConnection()
{
    printf("waiting for incoming connection...\n");
    LOG("waiting for incoming connection...\n");

    pollfd p_fd;
    p_fd.fd = socket_id;
    p_fd.events = POLLIN;

    bool ready_to_accept = false;

    while (not ready_to_accept)
    {
        int poll_res = poll(&p_fd, 1, 100000);
        if (poll_res == -1)
        {
            LOG("ERROR: socket pooling failed");
            return std::nullopt;
        }

        bool err_or_closed = ((p_fd.revents & POLLHUP) | (p_fd.revents & POLLERR)) ? true : false;
        if (err_or_closed)
        {
            LOG("Connection closed");
            return std::nullopt;
        }

        ready_to_accept = ((p_fd.revents & POLLIN) | (p_fd.revents & POLLPRI)) ? true : false;
        LOG("ready_to_accept = %d, p_fd.revents = %d", ready_to_accept, p_fd.revents);
    }
    return acceptConnection();
};

int ServerEndpoint::getClientHandle(size_t index)
{
    return client_info_storage[index].handle;
};

size_t ServerEndpoint::numOfConnections()
{
    return client_info_storage.size();
};
