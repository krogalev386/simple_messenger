#include "ServerTcpEndpoint.hpp"

#include "AuthentificationService.hpp"
#include "Logger.hpp"
#include "ThreadManager.hpp"

#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <netinet/in.h>
#include <optional>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

ServerTcpEndpoint::ServerTcpEndpoint(int port, bool blocking) : TcpEndpointBase(port, 0, blocking)
{
    client_info_storage = std::vector<ClientInfo>();
    address.sin_addr.s_addr = INADDR_ANY;
    status = bind(socket_id, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    if (status != 0)
    {
        perror("Error: socket binding failed");
    }
};

void ServerTcpEndpoint::listenConnections()
{
    status = listen(socket_id, maxTcpConnections);
    if (status != 0)
    {
        perror("Error: socket listening fails");
    }
};

void ServerTcpEndpoint::sendAcceptNotificaton(bool is_accepted, int client_socket_id)
{
    Envelope refuse_notification{};
    refuse_notification.meta_data.header.message_type = MessageType::ServiceMessage;
    char refuse_text[] = "REJECTED";
    char accept_text[] = "ACCEPTED";
    if (is_accepted)
    {
        memcpy(&refuse_notification.payload, &accept_text, sizeof(accept_text));
    }
    else
    {
        memcpy(&refuse_notification.payload, &refuse_text, sizeof(refuse_text));
    }
    sendEnvelope(refuse_notification, client_socket_id);
};

int ServerTcpEndpoint::acceptConnection()
{
    ClientInfo new_client{};
    int client_socket_id = 0;

    client_socket_id = accept4(socket_id, &new_client.socket_info.addr, &new_client.socket_info.addrlen, SOCK_CLOEXEC);

    LOG("client socket id: %d", client_socket_id);
    if (client_socket_id < 0)
    {
        perror("Error: acception failed");
        return client_socket_id;
    }

    ThreadManager::getInstance().schedule_task([this, new_client, client_socket_id]() mutable {
        // Authentification:
        if (authentificateUser(client_socket_id) == -1)
        {
            sendAcceptNotificaton(false, client_socket_id);
            close(client_socket_id);
            return;
        }

        sendAcceptNotificaton(true, client_socket_id);

        // Log in
        new_client.handle = client_socket_id;
        {
            // protect client_info_storage integrity
            std::unique_lock<std::mutex> lock(cis_mutex);
            client_info_storage.push_back(new_client);
            printf("Client fd %d is successfully registered\n", client_socket_id);
        }
        close(client_socket_id);
        printf("Connection with client_id %d closed.", client_socket_id);
    });
    return client_socket_id;
};

int ServerTcpEndpoint::authentificateUser(int client_socket_id)
{
    // Authentification:
    LOG("current client_socket_id: %d", client_socket_id);
    Envelope    credentials_envlp = receiveEnvelope(client_socket_id);
    MessageType msgType           = credentials_envlp.meta_data.header.message_type;

    if (msgType == MessageType::ServiceMessage)
    {
        UserCredentials credentials{};
        memcpy(&credentials, &(credentials_envlp.payload), sizeof(UserCredentials));
        LOG("Connection attempt from user %s, %s, socket %d", credentials.nickname, credentials.password, client_socket_id);
        bool is_registered = AuthentificationService::getInstance().checkIfRegistered(credentials);
        if (not is_registered)
        {
            const char* error_msg = "Error: authentification failed, no registered users found, connection closed";
            LOG(error_msg);
            perror(error_msg);
            return -1;
        }
    }
    else if (msgType == MessageType::UserMessage)
    {
        const char* error_msg = "Error: authentification failed, message of wrong type has been received, connection closed";
        LOG(error_msg);
        perror(error_msg);
        return -1;
    }
    return client_socket_id;
};

std::optional<int> ServerTcpEndpoint::tryAcceptConnection()
{
    const auto [ready_to_read, err_or_closed] = pollSocket(socket_id);
    if (ready_to_read)
    {
        return acceptConnection();
    }
    return std::nullopt;
};

int ServerTcpEndpoint::getClientHandle(size_t index)
{
    return client_info_storage[index].handle;
};

size_t ServerTcpEndpoint::numOfConnections() const
{
    return client_info_storage.size();
};
