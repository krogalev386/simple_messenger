#pragma once

#include "EndpointBase.hpp"

#include <vector>
#include <optional>
#include <mutex>

class ServerEndpoint : public EndpointBase
{
private:
    constexpr static uint16_t maxTcpConnections = 8;

public:
    ServerEndpoint(int port, int type, bool blocking);

    std::optional<int> tryAcceptConnection();

    void   listenConnections();
    int    acceptConnection();
    int    authentificateUser(int client_socket_id);
    int    getClientHandle(size_t index);
    size_t numOfConnections();

private:
    void sendAcceptNotificaton(bool is_accepted, int client_socket_id);

public:
    std::vector<ClientInfo> client_info_storage;
    std::mutex              cis_mutex;
};
