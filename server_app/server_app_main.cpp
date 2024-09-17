#include <cstdio>
#include <cstring>
#include <vector>

#include "ServerManager.hpp"

int main(int /*argn*/, char* /*argv*/[]) {
    ServerManager::getInstance().runEventLoop();
}