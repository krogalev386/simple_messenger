#pragma once

#include <mutex>
#include <optional>
#include <vector>

#include "TcpEndpointBase.hpp"
#include "defines.hpp"

class ServerTcpEndpoint : public TcpEndpointBase {
   private:
    constexpr static uint16_t maxTcpConnections = 8;

   public:
    ServerTcpEndpoint(int port, bool blocking);

    std::optional<int> tryAcceptConnection();

    void   listenConnections();
    int    acceptConnection();
    void   authentifyClient(ClientInfo&, int);
    size_t numOfConnections() const;
    int    getClientHandle(size_t);

    const std::vector<ClientInfo>& getConnectedClients() const;

   private:
    void sendAcceptNotificaton(int, std::optional<UserID>);

   public:
    std::vector<ClientInfo> client_info_storage;
    std::mutex              cis_mutex;
};
