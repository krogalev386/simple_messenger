#pragma once

#include "defines.hpp"

#include <vector>

namespace message_processing
{
    using namespace std;
    void         pack_message(const Envelope&, char* buffer);
    Envelope     unpack_message(const char* buffer, size_t len);
};
