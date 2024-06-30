#include "ClientEndpoint.hpp"

#include "defines.hpp"

#include <vector>
#include <cstring>
#include <cstdio>

void client_main()
{
    ClientEndpoint client_point(11111, SOCK_STREAM);
    client_point.connectTo("127.0.0.1", 11111);

    const char* greetings = "Hello,adfasdfadfaszxcvd dear serkver!";
    
    Envelope env = { 
        .meta_data = {
            .header = {
                .payload_size = 234,
                .total_size   = 400,
                .message_type = MessageType::UserMessage
            },
            .type         = DataType::TextMessage,
            .sender_id    = 4,
            .recipient_id = 2,
        }
    };
    memset(&env.payload, 0, Envelope::size - sizeof(EnvelopeMeta));
    memcpy(&env.payload, greetings, strlen(greetings));

    //std::vector<char> message(strlen(greetings)+1);
    //printf("Original message lenght: %ld\n", message.capacity());
    //std::memcpy(message.data(), greetings, message.capacity());

    //client_point.sendData(message);
    usleep(1000000);
    client_point.sendEnvelope(env);
    usleep(1000000);
    client_point.sendEnvelope(env);
    usleep(1000000);
    client_point.sendEnvelope(env);

};

int main(int argn, char* argv[])
{
    client_main();
}
