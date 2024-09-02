#include "ServerManager.hpp"

#include "AuthentificationService.hpp"
#include "Logger.hpp"
#include "ThreadManager.hpp"
#include "ServerTcpEndpoint.hpp"

#include <optional>
#include <cstdio>
#include <mutex>
#include <string.h>

ServerManager::ServerManager() : endpoint(11111, false), udpEndpoint(11112, false)
{
    AuthentificationService::init();
    ThreadManager::init();
};

ServerTcpEndpoint& ServerManager::getTcpEndPoint()
{
    return endpoint;
};

ServerUdpEndpoint& ServerManager::getUdpEndPoint()
{
    return udpEndpoint;
};

void ServerManager::checkMail()
{
    auto& udpEndpoint = getUdpEndPoint();

    std::optional<Envelope> result = udpEndpoint.tryReceiveEnvelope();
    if (result)
    {
        LOG("Received message over UDP: %s", result->payload);
        printf("Received message over UDP: %s\n", result->payload);
        MessageType msgType = result->meta_data.header.message_type;
        if (msgType == MessageType::UserMessage)
        {
            LOG("User message received");
        }
    }
};

void ServerManager::runEventLoop()
{
    auto& endpoint = getTcpEndPoint();

    endpoint.listenConnections();

    while(true)
    {
        endpoint.tryAcceptConnection();
        checkMail();
    }
};
