#include "MessageProcessing.hpp"

#include "defines.hpp"

#include <vector>
#include <cstring>

namespace message_processing
{
    using namespace std;

    void pack_message(const Envelope& envelope, char* buffer)
    {
        vector<char> message_data(Envelope::size);
        memcpy(buffer, &envelope, Envelope::size);
    };

    Envelope unpack_message(const char* buffer, size_t len)
    {
        Envelope envelope;
        memset(&envelope, 0, Envelope::size);
        memcpy(&envelope, buffer, len);
        return std::move(envelope);
    };
};
