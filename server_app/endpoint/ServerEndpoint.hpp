#pragma once

#include "EndpointBase.hpp"

#include <vector>
#include <optional>

class ServerEndpoint : public EndpointBase
{
public:
    ServerEndpoint(int port, int type, bool blocking);
    void listenConnections(int queue_size);
    int acceptConnection();
    std::optional<int> tryAcceptConnection();
    int getClientHandle(size_t index);
    size_t numOfConnections();

public:
    std::vector<ClientInfo> client_info_storage;
};
