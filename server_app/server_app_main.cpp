#include "ServerTcpEndpoint.hpp"
#include "ServerManager.hpp"
#include "MessageProcessing.hpp"

#include <vector>
#include <cstring>
#include <cstdio>

int main(int argn, char* argv[])
{
    ServerManager::getInstance().runEventLoop();
}