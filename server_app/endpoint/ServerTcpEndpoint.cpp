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

void ServerTcpEndpoint::sendAcceptNotificaton(bool   is_accepted,
                                              int    client_socket_id,
                                              UserID user_id) {
    Envelope auth_notification{};
    msg_proc::setMessageType(auth_notification, MessageType::ServiceMessage);

    AuthResponse resp{is_accepted, user_id};
    msg_proc::set_payload<AuthResponse>(auth_notification, resp);
    sendEnvelope(auth_notification, client_socket_id);
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
            // Authentification:
            auto user_id = authentificateUser(client_socket_id);
            if (not user_id) {
                sendAcceptNotificaton(false, client_socket_id, 0);
                close(client_socket_id);
                return;
            }

            sendAcceptNotificaton(true, client_socket_id, user_id.value());

            // Log in
            new_client.handle = client_socket_id;
            {
                // protect client_info_storage integrity
                std::unique_lock<std::mutex> lock(cis_mutex);
                client_info_storage.push_back(new_client);
                printf("Client fd %d is successfully registered\n",
                       client_socket_id);
            }
            close(client_socket_id);
            printf("Connection with client_id %d closed.", client_socket_id);
        });
    return client_socket_id;
};

std::optional<UserID> ServerTcpEndpoint::authentificateUser(
    int client_socket_id) {
    // Authentification:
    LOG("current client_socket_id: %d", client_socket_id);
    Envelope    credentials_envlp = receiveEnvelope(client_socket_id);
    MessageType msgType           = msg_proc::getMessageType(credentials_envlp);

    if (msgType == MessageType::ServiceMessage) {
        auto credentials =
            msg_proc::get_payload<UserCredentials>(credentials_envlp);
        LOG("Connection attempt from user %s, %s, socket %d", credentials.email,
            credentials.password, client_socket_id);

        std::optional<UserID> user_id =
            AuthentificationService::getInstance().checkIfRegistered(
                credentials);

        if (not user_id) {
            const char* error_msg =
                "Error: authentification failed, no registered users found, "
                "connection closed";
            LOG(error_msg);
            perror(error_msg);
        }
        return user_id;
    }
    if (msgType == MessageType::UserMessage) {
        const char* error_msg =
            "Error: authentification failed, message of wrong type has been "
            "received, connection closed";
        LOG(error_msg);
        perror(error_msg);
        return std::nullopt;
    }
    return std::nullopt;
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
