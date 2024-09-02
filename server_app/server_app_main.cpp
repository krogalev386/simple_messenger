#include <cstdio>
#include <cstring>
#include <vector>

#include "MessageProcessing.hpp"
#include "ServerManager.hpp"
#include "ServerTcpEndpoint.hpp"

int main(int /*argn*/, char* /*argv*/[]) {
    ServerManager::getInstance().runEventLoop();
}