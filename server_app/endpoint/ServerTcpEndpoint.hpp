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
    size_t numOfConnections() const;
    int    getClientHandle(size_t index);

    std::optional<UserID> authentificateUser(int client_socket_id);

   private:
    void sendAcceptNotificaton(bool is_accepted, int client_socket_id,
                               UserID user_id);

   public:
    std::vector<ClientInfo> client_info_storage;
    std::mutex              cis_mutex;
};
