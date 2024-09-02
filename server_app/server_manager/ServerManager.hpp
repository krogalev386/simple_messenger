#pragma once

#include "CrtpSingleton.hpp"
#include "ServerTcpEndpoint.hpp"
#include "ServerUdpEndpoint.hpp"

class ServerManager : public CrtpSingleton<ServerManager> {
    friend class CrtpSingleton<ServerManager>;

   protected:
    ServerManager();
    ~ServerManager() = default;

   public:
    ServerTcpEndpoint& getTcpEndPoint();
    ServerUdpEndpoint& getUdpEndPoint();

    void runEventLoop();
    void checkMail();

   private:
    ServerTcpEndpoint endpoint;
    ServerUdpEndpoint udpEndpoint;
};
