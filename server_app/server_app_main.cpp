#include "ServerTcpEndpoint.hpp"
#include "MessageProcessing.hpp"
#include "ServerManager.hpp"

#include <cstdio>
#include <cstring>
#include <vector>

int main(int  /*argn*/, char*  /*argv*/[])
{
    ServerManager::getInstance().runEventLoop();
}