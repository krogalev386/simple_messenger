#include "ServerTcpEndpoint.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <optional>
#include <vector>

#include "AuthentificationService.hpp"
#include "Logger.hpp"
#include "MessageProcessing.hpp"
#include "ThreadManager.hpp"
#include "defines.hpp"

ServerTcpEndpoint::ServerTcpEndpoint(int port, bool blocking)
    : TcpEndpointBase(port, 0, blocking) {
    client_info_storage     = std::vector<ClientInfo>();
    address.sin_addr.s_addr = INADDR_ANY;
    status =
        bind(socket_id, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    if (status != 0) {
        perror("Error: socket binding failed");
    }
};

void ServerTcpEndpoint::listenConnections() {
    status = listen(socket_id, maxTcpConnections);
    if (status != 0) {
        perror("Error: socket listening fails");
    }
};

void ServerTcpEndpoint::sendAcceptNotificaton(
    int client_socket_id, std::optional<UserID> user_id_opt) {
    Envelope auth_notification{};
    msg_proc::setMessageType(auth_notification, MessageType::ServiceMessage);
    UserID       u_id        = user_id_opt ? user_id_opt.value() : 0;
    bool         is_accepted = static_cast<bool>(user_id_opt);
    AuthResponse resp{is_accepted, u_id};
    msg_proc::set_payload<AuthResponse>(auth_notification, resp);
    sendEnvelope(auth_notification, client_socket_id);
};

void ServerTcpEndpoint::authentifyClient(ClientInfo& client_info, int sock_id) {
    // Authentification:
    Envelope authReqEnv = receiveEnvelope(sock_id);
    auto     user_id =
        AuthentificationService::getInstance().handleAuthReq(authReqEnv);

    sendAcceptNotificaton(sock_id, user_id);

    if (not user_id) {
        close(sock_id);
        return;
    }

    // Log in
    client_info.handle = sock_id;
    {
        // protect client_info_storage integrity
        std::unique_lock<std::mutex> lock(cis_mutex);
        client_info_storage.push_back(client_info);
        printf("Client fd %d is successfully registered\n", sock_id);
    }
    close(sock_id);
    printf("Connection with client_id %d closed.", sock_id);
};

int ServerTcpEndpoint::acceptConnection() {
    ClientInfo new_client{};
    int        client_socket_id = 0;
    client_socket_id = accept4(socket_id, &new_client.socket_info.addr,
                               &new_client.socket_info.addrlen, SOCK_CLOEXEC);

    reinterpret_cast<sockaddr_in*>(&new_client.socket_info.addr)->sin_port =
        htons(DEFAUILT_UDP_PORT);

    LOG("client socket id: %d", client_socket_id);
    if (client_socket_id < 0) {
        perror("Error: acception failed");
        return client_socket_id;
    }

    ThreadManager::getInstance().schedule_task(
        [this, new_client, client_socket_id]() mutable {
            authentifyClient(new_client, client_socket_id);
        });
    return client_socket_id;
};

std::optional<int> ServerTcpEndpoint::tryAcceptConnection() {
    const auto [ready_to_read, err_or_closed] = pollSocket(socket_id);
    if (ready_to_read) {
        return acceptConnection();
    }
    return std::nullopt;
};

int ServerTcpEndpoint::getClientHandle(size_t index) {
    return client_info_storage[index].handle;
};

size_t ServerTcpEndpoint::numOfConnections() const {
    return client_info_storage.size();
};

const std::vector<ClientInfo>& ServerTcpEndpoint::getConnectedClients() const {
    return client_info_storage;
}
