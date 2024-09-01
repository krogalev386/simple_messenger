#pragma once

#include "UdpEndpointBase.hpp"

#include <vector>
#include <optional>
#include <mutex>

class ServerUdpEndpoint : public UdpEndpointBase
{
public:
    ServerUdpEndpoint(int port, bool blocking);
};
