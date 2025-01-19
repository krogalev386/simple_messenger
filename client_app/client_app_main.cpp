#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

#include "ClientTcpEndpoint.hpp"
#include "ClientUdpEndpoint.hpp"
#include "Logger.hpp"
#include "MessageProcessing.hpp"
#include "ThreadManager.hpp"
#include "defines.hpp"

constexpr size_t buffer_size = Envelope::size - sizeof(EnvelopeMeta);

Envelope create_text_envelope(UserID sender_id, UserID recepient_id) {
    Envelope env = {
        .meta_data = {
                      .header       = {.payload_size = 234,
                             .total_size   = 400,
                             .message_type = MessageType::UserMessage},
                      .type         = DataType::TextMessage,
                      .sender_id    = sender_id,
                      .recepient_id = recepient_id,
                      }
    };
    return env;
}

Envelope create_auth_envelope() {
    Envelope env = create_text_envelope(0, 0);
    msg_proc::setMessageType(env, MessageType::ServiceMessage);
    return env;
}

void embed_text(Envelope& env, char* text_buffer) {
    memcpy(&env.payload, text_buffer, buffer_size);
}

void write_message(char* text_buffer) {
    memset(text_buffer, 0, buffer_size);
    scanf("%s", text_buffer);
}

#if 0
void client_main()
{
    ClientTcpEndpoint client_tcp_point(DEFAUILT_TCP_PORT);
    client_tcp_point.connectTo("127.0.0.1", DEFAUILT_TCP_PORT);

    const char* greetings = "Hello,adfasdfadfaszxcvd dear serkver!";
    
    Envelope env = create_text_envelope(0,0);

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

UserCredentials user_cred[] = {
    {"user1@mail.com", "user1_pass"},
    {"user2@mail.com", "user2_pass"},
    {"user3@mail.com", "user3_pass"}
};

void client_interactive_main(size_t port_id, const char* ip_string,
                             int user_num) {
    // Connection:
    ClientTcpEndpoint client_tcp_point(port_id);
    ClientUdpEndpoint client_udp_point(DEFAUILT_UDP_PORT);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(DEFAUILT_UDP_PORT);
    inet_pton(AF_INET, ip_string, &server_addr.sin_addr);

    SocketInfo serv_addr{*reinterpret_cast<sockaddr*>(&server_addr),
                         sizeof(server_addr)};
    client_tcp_point.connectTo(ip_string, port_id);

    Envelope auth_message = create_auth_envelope();
    msg_proc::set_payload<UserCredentials>(auth_message, user_cred[user_num]);

    client_tcp_point.sendEnvelope(auth_message);

    Envelope acknowledge_env = client_tcp_point.receiveEnvelope();
    if (msg_proc::getMessageType(acknowledge_env) !=
        MessageType::ServiceMessage) {
        printf("Connection refused, wrong message type, please try again\n");
        return;
    }

    auto resp        = msg_proc::get_payload<AuthResponse>(acknowledge_env);
    bool is_accepted = resp.is_accepted;

    if (not is_accepted) {
        printf("Connection refused, please try again\n");
        return;
    }
    printf("Access granted, your user ID is %lu\n", resp.user_id);
    UserID my_id = resp.user_id;
    // Job loops
    char   buffer[buffer_size];
    char   resp_buffer[buffer_size];
    memset(buffer, 0, sizeof(buffer));
    memset(resp_buffer, 0, sizeof(resp_buffer));

    bool isRunning = true;

    // Listening
    auto listen = [&isRunning, &client_udp_point, &serv_addr,
                   resp_buffer]() mutable {
        while (isRunning) {
            std::optional<Envelope> result =
                client_udp_point.tryReceiveEnvelope();
            if (result) {
                memcpy(resp_buffer, &(result->payload), sizeof(resp_buffer));
                printf("Ping from server received: %s\n", resp_buffer);
            }
        }
    };

    // Sending
    auto send = [&isRunning, &client_udp_point, &serv_addr, &buffer,
                 &my_id]() mutable {
        UserID recepient_id = 0;
        printf("Who are you going to send to?\n");
        scanf("%lu", &recepient_id);
        printf("Recepient ID: %lu\n", recepient_id);
        while (not(strcmp(buffer, "exit") == 0)) {
            Envelope env = create_text_envelope(my_id, recepient_id);
            write_message(buffer);
            embed_text(env, buffer);
            client_udp_point.sendEnvelope(env, serv_addr);
        }
        isRunning = false;
    };

    // Run concurrent threads
    std::thread listeing_thrd = std::thread(listen);
    std::thread sending_thrd  = std::thread(send);

    // Shutdown
    listeing_thrd.join();
    sending_thrd.join();
}

int main(int argn, char* argv[]) {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (argn == 2) {
        client_interactive_main(DEFAUILT_TCP_PORT, "127.0.0.1",
                                std::atoi(argv[1]));
    } else if (argn == 3) {
        client_interactive_main(DEFAUILT_TCP_PORT, argv[1], std::atoi(argv[2]));
    } else {
        printf("Too many args; the client has not started\n");
    }

    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();
}
