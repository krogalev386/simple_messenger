#include "ClientTcpEndpoint.hpp"
#include "ClientUdpEndpoint.hpp"

#include "defines.hpp"

#include <cstdio>
#include <cstring>
#include <vector>

constexpr size_t buffer_size = Envelope::size - sizeof(EnvelopeMeta);

Envelope create_text_envelope()
{
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
    return env;
}

Envelope create_auth_envelope()
{
    Envelope env = create_text_envelope();
    env.meta_data.header.message_type = MessageType::ServiceMessage;
    return env;
}

void embed_text(Envelope& env, char* text_buffer)
{
    memcpy(&env.payload, text_buffer, buffer_size);
}

void write_message(char* text_buffer)
{
    memset(text_buffer, 0, buffer_size);
    scanf("%s", text_buffer);
}

#if 0
void client_main()
{
    ClientTcpEndpoint client_tcp_point(11111);
    client_tcp_point.connectTo("127.0.0.1", 11111);

    const char* greetings = "Hello,adfasdfadfaszxcvd dear serkver!";
    
    Envelope env = create_text_envelope();

    memset(&env.payload, 0, buffer_size);
    memcpy(&env.payload, greetings, strlen(greetings));

    usleep(1000000);
    client_tcp_point.sendEnvelope(env);
    usleep(1000000);
    client_tcp_point.sendEnvelope(env);
    usleep(1000000);
    client_tcp_point.sendEnvelope(env);
};
#endif

void client_interactive_main(size_t port_id, const char* ip_string)
{
    // Connection:
    ClientTcpEndpoint client_tcp_point(port_id);
    ClientUdpEndpoint client_udp_point(11112);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(11112);
    inet_pton(AF_INET, ip_string, &server_addr.sin_addr);

    SocketInfo serv_addr {*reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)};
    client_tcp_point.connectTo(ip_string, port_id);


    // Authentification:
    UserCredentials credentials{};

    printf("Enter your credentials\n");
    printf("Enter your username:\n");
    scanf("%s", credentials.nickname);
    printf("Enter your password:\n");
    scanf("%s", credentials.password);

    Envelope auth_message = create_auth_envelope();
    memcpy(&auth_message.payload, &credentials, sizeof(UserCredentials));

    client_tcp_point.sendEnvelope(auth_message);

    Envelope acknowledge_env = client_tcp_point.receiveEnvelope();
    if (acknowledge_env.meta_data.header.message_type != MessageType::ServiceMessage)
    {
        printf("Connection refused, wrong message type, please try again\n");
        return;
    }

    bool is_accepted = (strcmp(acknowledge_env.payload, "ACCEPTED") == 0);

    if (not is_accepted)
    {
        printf("Connection refused, please try again\n");
        return;
    }

    // Job loop
    char buffer[buffer_size];
    memset(buffer, 0, sizeof(buffer));

    while (not (strcmp(buffer, "exit") == 0))
    {
        Envelope env = create_text_envelope();
        write_message(buffer);
        embed_text(env, buffer);
        client_udp_point.sendEnvelope(env, serv_addr);
    }
}

int main(int argn, char* argv[])
{
    if (argn == 1)
    {
        client_interactive_main(11111,"127.0.0.1");
    }
    else if (argn == 2)
    {
        client_interactive_main(11111, argv[1]);
    }
    else
    {
        printf("Too many args; the client has not started\n");
    }
}
