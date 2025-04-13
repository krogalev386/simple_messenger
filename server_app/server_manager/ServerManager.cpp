#include "ServerManager.hpp"

#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server_builder.h>

#include <cstdio>
#include <cstring>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include "AuthentificationService.hpp"
#include "GrpcServer.hpp"
#include "Logger.hpp"
#include "MessageProcessing.hpp"
#include "PsqlManager.hpp"
#include "ServerTcpEndpoint.hpp"
#include "ThreadManager.hpp"

ServerManager::ServerManager()
    : endpoint(DEFAUILT_TCP_PORT, false), udpEndpoint(DEFAUILT_UDP_PORT, true) {
    AuthentificationService::init();
    PsqlManager::init();

    ///////////////////////////////////////////
    // Predefined users for testing purposes
    auto& psql_db = PsqlManager::getInstance();

    psql_db.tryConnect("postgres", "admin", "11.0.0.3", "5432");
    if (psql_db.isConnected()) {
        psql_db.createUsersTableIfNotExist();
        psql_db.registerNewUser("user1", "user1_pass", "user1@mail.com");
        psql_db.registerNewUser("user2", "user2_pass", "user2@mail.com");
    }
    ///////////////////////////////////////////

    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    ThreadManager::init();
};

ServerManager::~ServerManager() {
    AuthentificationService::destroy();
    ThreadManager::destroy();

    // Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();
}
ServerTcpEndpoint& ServerManager::getTcpEndPoint() { return endpoint; };

ServerUdpEndpoint& ServerManager::getUdpEndPoint() { return udpEndpoint; };

std::optional<ClientInfo> ServerManager::findOnlineClient(
    const UserID& user_id) {
    for (auto connection : getTcpEndPoint().getConnectedClients()) {
        if (connection.user_id == user_id) {
            return connection;
        }
    }
    return std::nullopt;
}

void ServerManager::checkMail() {
    auto& udpEndpoint = getUdpEndPoint();

    auto report_result = [](const Envelope& result) {
        LOG("Received message over UDP: %s", result.payload);
        printf("Received message over UDP: %s\n", result.payload);
        MessageType msgType = msg_proc::getMessageType(result);
        if (msgType == MessageType::UserMessage) {
            LOG("User message received");
        }
    };
#if 1
    std::optional<Envelope> result = udpEndpoint.tryReceiveEnvelope();
    if (result) {
        report_result(*result);
        UserID rec_id   = msg_proc::getRecepientID(*result);
        auto   rec_info = findOnlineClient(rec_id);
        if (rec_info) {
            udpEndpoint.sendEnvelope(*result, rec_info->socket_info);
        } else {
            LOG("Message transmission failed: recepient is offline");
        }
    }
#else
    std::optional<Envelope> result = udpEndpoint.tryReceiveEnvelope();
    if (result) {
        report_result(*result);
    }
#endif
};

void ServerManager::runEventLoop() {
    std::string addr =
        std::string("0.0.0.0:") + std::to_string(DEFAUILT_GRPC_PORT);
    GrpcEndpointServer  grpc_endpoint;
    grpc::ServerBuilder builder;
    builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&grpc_endpoint);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    LOG("gRPC endpoint is up...");
    server->Wait();
};
