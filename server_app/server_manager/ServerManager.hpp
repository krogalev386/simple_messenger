#pragma once

#include "ServerEndpoint.hpp"

#include "AuthentificationService.hpp"

class ServerManager
{
private:
    ServerManager();
    ~ServerManager()                    = default;
    ServerManager(const ServerManager&) = delete;
    ServerManager(ServerManager&&)      = delete;

public:
    static ServerManager& getInstance()
    {
        static ServerManager instance;
        return instance;
    }

    ServerEndpoint& getEndPoint();

    void runEventLoop();
    void checkMail();

private:
    ServerEndpoint endpoint;
};
