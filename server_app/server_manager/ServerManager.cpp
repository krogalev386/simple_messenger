#include "ServerManager.hpp"

#include "Logger.hpp"
#include "ServerEndpoint.hpp"

#include <optional>
#include <cstdio>
#include <string.h>

ServerManager::ServerManager() : endpoint(11111, SOCK_STREAM, false)
{};

ServerEndpoint& ServerManager::getEndPoint()
{
    return endpoint;
};

void ServerManager::checkMail()
{
    for(auto client_it = endpoint.client_info_storage.begin();
                client_it != endpoint.client_info_storage.end();
                client_it++)
    {
        int client_fd = client_it->handle;
        std::optional<Envelope> result = endpoint.tryReceiveEnvelope(client_fd);
        if (result)
        {
            LOG("Client %d, Received message: %s", client_fd, result->payload);
            printf("Received message: %s\n", result->payload);
            if ((result->meta_data.header.message_type == MessageType::ServiceMessage)
            and strcmp(result->payload, "ConnectionClosed") == 0)
            {
                endpoint.client_info_storage.erase(client_it);
                client_it--;
            }
        }
        //printf("Num of connections: %ld\n", endpoint.numOfConnections());
    }
};

void ServerManager::runMainThread()
{
    auto& endpoint = getEndPoint();

    endpoint.listenConnections(5);

    while(true)
    {
        endpoint.tryAcceptConnection();
        checkMail();
    }
};
