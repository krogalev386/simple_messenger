#pragma once

#include <mutex>
#include <optional>
#include <vector>

#include "TcpEndpointBase.hpp"

class ServerTcpEndpoint : public TcpEndpointBase {
   private:
    constexpr static uint16_t maxTcpConnections = 8;

   public:
    ServerTcpEndpoint(int port, bool blocking);

    std::optional<int> tryAcceptConnection();

    void   listenConnections();
    int    acceptConnection();
    int    authentificateUser(int client_socket_id);
    int    getClientHandle(size_t index);
    size_t numOfConnections() const;

   private:
    void sendAcceptNotificaton(bool is_accepted, int client_socket_id);

   public:
    std::vector<ClientInfo> client_info_storage;
    std::mutex              cis_mutex;
};
