#include "ClientTcpEndpoint.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>

void ClientTcpEndpoint::connectTo(const char* ip_address, int server_port)
{
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(server_port);
    inet_pton(AF_INET, ip_address, &server_addr.sin_addr);
    status = connect(socket_id, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
    if (status)
    {
        perror("Error: client connection failed");
    }
    printf("connection is established\n");
};
