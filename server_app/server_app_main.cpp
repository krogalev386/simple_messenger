#include "ServerEndpoint.hpp"
#include "ServerManager.hpp"
#include "MessageProcessing.hpp"

#include <vector>
#include <cstring>
#include <cstdio>

int main(int argn, char* argv[])
{
    ServerManager& serverManager = ServerManager::getInstance();

    serverManager.runMainThread();
    //int client_fd = server_point.getClientHandle(0);
    //std::vector<char> received_data = server_point.receiveData(client_fd);
    //Envelope env = server_point.receiveEnvelope(client_fd);

    //printf("Message size: %ld\n", received_data.size());
    //printf("%s\n", reinterpret_cast<char*>(received_data.data()));
    //printf("%s\n", env.payload);
}