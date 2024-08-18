#pragma once

#include "EndpointBase.hpp"

#include <vector>
#include <optional>

class ServerEndpoint : public EndpointBase
{
private:
    constexpr static uint16_t maxTcpConnections = 8;

public:
    ServerEndpoint(int port, int type, bool blocking);

    std::optional<int> tryAcceptConnection();

    void   listenConnections();
    int    acceptConnection();
    int    getClientHandle(size_t index);
    size_t numOfConnections();

public:
    std::vector<ClientInfo> client_info_storage;
};
