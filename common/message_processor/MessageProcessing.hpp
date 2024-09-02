#pragma once

#include <vector>

#include "defines.hpp"

namespace message_processing {
using namespace std;
void     pack_message(const Envelope&, char* buffer);
Envelope unpack_message(const char* buffer, size_t len);
};  // namespace message_processing
