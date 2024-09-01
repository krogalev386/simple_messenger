#include "ServerManager.hpp"

#include "AuthentificationService.hpp"
#include "Logger.hpp"
#include "ThreadManager.hpp"
#include "ServerEndpoint.hpp"

#include <optional>
#include <cstdio>
#include <mutex>
#include <string.h>

ServerManager::ServerManager() : endpoint(11111, SOCK_STREAM, false)
{
    AuthentificationService::init();
    ThreadManager::init();
};

ServerEndpoint& ServerManager::getEndPoint()
{
    return endpoint;
};

void ServerManager::checkMail()
{
    // Protect client_info_storage integrity
    std::unique_lock<std::mutex> lock(endpoint.cis_mutex);

    for(auto client_it  = endpoint.client_info_storage.begin();
             client_it != endpoint.client_info_storage.end();
             client_it++)
    {
        int client_fd = client_it->handle;
        std::optional<Envelope> result = endpoint.tryReceiveEnvelope(client_fd);
        if (result)
        {
            LOG("Client %d, Received message: %s", client_fd, result->payload);
            printf("Received message: %s\n", result->payload);
            MessageType msgType = result->meta_data.header.message_type;
            if (msgType == MessageType::UserMessage)
            {
                LOG("User message received");
            }
            if ((msgType == MessageType::ServiceMessage)
                 and strcmp(result->payload, "ConnectionClosed") == 0)
            {
                close(client_fd);
                endpoint.client_info_storage.erase(client_it);
                client_it--;
            }
        }
    }
};

void ServerManager::runEventLoop()
{
    auto& endpoint = getEndPoint();

    endpoint.listenConnections();

    while(true)
    {
        endpoint.tryAcceptConnection();
        checkMail();
    }
};
