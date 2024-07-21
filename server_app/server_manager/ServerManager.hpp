#pragma once

#include "ServerEndpoint.hpp"

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

    void runEventLoop();
    ServerEndpoint& getEndPoint();
    void checkMail();

private:
    ServerEndpoint endpoint;
};
