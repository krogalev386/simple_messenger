#include "ServerManager.hpp"

#include <cstdio>
#include <cstring>
#include <mutex>
#include <optional>

#include "AuthentificationService.hpp"
#include "Logger.hpp"
#include "PsqlManager.hpp"
#include "ServerTcpEndpoint.hpp"
#include "ThreadManager.hpp"

ServerManager::ServerManager()
    : endpoint(11111, false), udpEndpoint(11112, false) {
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

    std::optional<Envelope> result = udpEndpoint.tryReceiveEnvelope();
    if (result) {
        LOG("Received message over UDP: %s", result->payload);
        printf("Received message over UDP: %s\n", result->payload);
        MessageType msgType = result->meta_data.header.message_type;
        if (msgType == MessageType::UserMessage) {
            LOG("User message received");
        }
    }
};

void ServerManager::runEventLoop() {
    auto& endpoint = getTcpEndPoint();
    endpoint.listenConnections();

    while (true) {
        endpoint.tryAcceptConnection();
        checkMail();
    }
};
