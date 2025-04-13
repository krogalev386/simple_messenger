#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

#include "ClientTcpEndpoint.hpp"
#include "ClientUdpEndpoint.hpp"
#include "GrpcClient.hpp"
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

UserCredentials user_cred[] = {
    {"user1@mail.com", "user1_pass"},
    {"user2@mail.com", "user2_pass"},
    {"user3@mail.com", "user3_pass"}
};

void grpc_client(size_t port_id, const char* ip_string, int user_num) {
    std::string addr =
        std::string(ip_string) + std::string(":") + std::to_string(port_id);
    GrpcEndpointClient client(
        grpc::CreateChannel(addr, grpc::InsecureChannelCredentials()),
        "adsfadsf");

    bool is_connected = client.connectToServer(user_cred[user_num]);
    if (not is_connected) {
        printf("Connection refused, please try again\n");
        return;
    };
    printf("Access granted, your user ID is %lu\n", client.getUserID());
    UserID my_id = client.getUserID();

    // Job loops
    char buffer[buffer_size];
    char resp_buffer[buffer_size];
    memset(buffer, 0, sizeof(buffer));
    memset(resp_buffer, 0, sizeof(resp_buffer));

    // Listening
    auto listen = [&client, resp_buffer]() mutable {
        client.listenOthers();
    };

    // Sending
    auto send = [&buffer, &my_id, &client]() mutable {
        UserID recepient_id = 0;
        printf("Who are you going to send to?\n");
        scanf("%lu", &recepient_id);
        printf("Recepient ID: %lu\n", recepient_id);
        protobuf_itf::Envelope proto_env;
        while (not(strcmp(buffer, "exit") == 0)) {
            Envelope env = create_text_envelope(my_id, recepient_id);
            write_message(buffer);
            embed_text(env, buffer);
            msg_proc::putEnvToProtoBuf(env, proto_env);
            client.sendEnvelope(proto_env);
        }
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

    if (argn == 3) {
        grpc_client(DEFAUILT_GRPC_PORT, argv[1], std::atoi(argv[2]));
    } else {
        printf("Too many or too few args; the client has not started\n");
    }

    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();
}
