#pragma once

#include <mutex>
#include <optional>
#include <vector>

#include "UdpEndpointBase.hpp"

class ServerUdpEndpoint : public UdpEndpointBase {
   public:
    ServerUdpEndpoint(int port, bool blocking);
};
