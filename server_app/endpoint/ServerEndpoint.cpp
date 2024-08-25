#include "ServerEndpoint.hpp"

#include "AuthentificationService.hpp"
#include "Logger.hpp"
#include "ThreadManager.hpp"

#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <optional>
#include <poll.h>
#include <netinet/in.h>
#include <vector>
#include <sys/socket.h>
#include <unistd.h>

ServerEndpoint::ServerEndpoint(int port, int type, bool blocking) : EndpointBase(port, type, 0, blocking)
{
    client_info_storage = std::vector<ClientInfo>();
    address.sin_addr.s_addr = INADDR_ANY;
    status = bind(socket_id, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    if (status)
    {
        perror("Error: socket binding failed");
    }
};

void ServerEndpoint::listenConnections()
{
    status = listen(socket_id, maxTcpConnections);
    if (status)
    {
        perror("Error: socket listening fails");
    }
};

int ServerEndpoint::acceptConnection()
{
    ClientInfo new_client;
    int client_socket_id;

    client_socket_id = accept(socket_id, &new_client.addr, &new_client.addrlen);
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
            close(client_socket_id);
            return;
        }

        // Log in
        new_client.handle = client_socket_id;
        {
            // protect client_info_storage integrity
            std::unique_lock<std::mutex> lock(cis_mutex);
            client_info_storage.push_back(new_client);
            printf("Client fd %d is successfully registered\n", client_socket_id);
        }
    });
    return client_socket_id;
};

int ServerEndpoint::authentificateUser(int client_socket_id)
{
    // Authentification:
    LOG("current client_socket_id: %d", client_socket_id);
    Envelope    credentials_envlp = receiveEnvelope(client_socket_id);
    MessageType msgType           = credentials_envlp.meta_data.header.message_type;

    if (msgType == MessageType::ServiceMessage)
    {
        UserCredentials credentials;
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

std::optional<int> ServerEndpoint::tryAcceptConnection()
{
    const auto [ready_to_read, err_or_closed] = pollSocket(socket_id);
    if (ready_to_read)
    {
        return acceptConnection();
    }
    else
    {
        return std::nullopt;
    }
};

int ServerEndpoint::getClientHandle(size_t index)
{
    return client_info_storage[index].handle;
};

size_t ServerEndpoint::numOfConnections()
{
    return client_info_storage.size();
};
