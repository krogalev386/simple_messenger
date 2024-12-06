#pragma once

#include "ServerTcpEndpoint.hpp"
#include "ServerUdpEndpoint.hpp"
#include "StaticCrtpSingleton.hpp"

class ServerManager : public StaticCrtpSingleton<ServerManager> {
    friend class StaticCrtpSingleton<ServerManager>;

   protected:
    ServerManager();
    ~ServerManager();

   private:
    std::optional<ClientInfo> findOnlineClient(const UserID&);

   public:
    ServerTcpEndpoint& getTcpEndPoint();
    ServerUdpEndpoint& getUdpEndPoint();

    void runEventLoop();
    void checkMail();

   private:
    ServerTcpEndpoint endpoint;
    ServerUdpEndpoint udpEndpoint;
};
