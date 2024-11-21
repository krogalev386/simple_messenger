#include "ServerManager.hpp"

#include <cstdio>
#include <cstring>
#include <mutex>
#include <optional>

#include "AuthentificationService.hpp"
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

    ThreadManager::init();
};

ServerManager::~ServerManager() {
    AuthentificationService::destroy();
    ThreadManager::destroy();
}
ServerTcpEndpoint& ServerManager::getTcpEndPoint() { return endpoint; };

ServerUdpEndpoint& ServerManager::getUdpEndPoint() { return udpEndpoint; };

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
        for (auto connection : getTcpEndPoint().getConnectedClients()) {
            udpEndpoint.sendEnvelope(*result, connection.socket_info);
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
    auto& endpoint = getTcpEndPoint();
    endpoint.listenConnections();

    while (true) {
        endpoint.tryAcceptConnection();
        checkMail();
    }
};
