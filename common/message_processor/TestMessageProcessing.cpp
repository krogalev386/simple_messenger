#include "MessageProcessing.hpp"

bool testProcessing() {
    Envelope env = {
        .meta_data =
            {
                        .header       = {.payload_size = 234,
                                 .total_size   = 400,
                                 .message_type = MessageType::UserMessage},
                        .type         = DataType::TextMessage,
                        .sender_id    = 4,
                        .recipient_id = 2,
                        }
    } memset(&env.payload, 0, Envelope::size - sizeof(EnvelopeMeta));

    const char* text = "Hi!";
    memcpy(&env.payload, text, strlen(text));

    std::vector<char> serialized(Envelope::size);
    message_processing::pack_message(env, serialized.data());
    Envelope deserialized =
        message_processing::unpack_message(serialized.data(), Envelope::size);

    return memcmp(&env, &deserialized, sizeof(Envelope)) == 0;
};
