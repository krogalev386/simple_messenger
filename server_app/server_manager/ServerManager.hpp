#pragma once

#include "CrtpSingleton.hpp"
#include "ServerEndpoint.hpp"

class ServerManager : public CrtpSingleton<ServerManager>
{
friend class CrtpSingleton<ServerManager>;

protected:
    ServerManager();
    ~ServerManager() = default;

public:
    ServerEndpoint& getEndPoint();

    void runEventLoop();
    void checkMail();

private:
    ServerEndpoint endpoint;
};
