#include "AuthentificationService.hpp"

#include <algorithm>
#include <cstring>
#include <optional>

#include "Logger.hpp"
#include "MessageProcessing.hpp"
#include "PsqlManager.hpp"
#include "defines.hpp"

AuthentificationService::AuthentificationService() = default;

std::optional<UserID> AuthentificationService::checkIfRegistered(
    const UserCredentials& considered_cred) {
    auto& psql_db = PsqlManager::getInstance();

    if (!psql_db.isConnected()) {
        printf("Authentification error: no connection to database\n");
        return std::nullopt;
    }

    auto user_id = PsqlManager::getInstance().findUserByCreds(considered_cred);
    return user_id;
}

std::optional<UserID> AuthentificationService::handleAuthReq(
    const Envelope& authReqEnv) {
    // Authentification:
    MessageType msgType = msg_proc::getMessageType(authReqEnv);

    if (msgType == MessageType::ServiceMessage) {
        auto credentials = msg_proc::get_payload<UserCredentials>(authReqEnv);
        LOG("Connection attempt from user %s, %s", credentials.email,
            credentials.password);

        std::optional<UserID> user_id = checkIfRegistered(credentials);

        if (not user_id) {
            const char* error_msg =
                "Error: authentification failed, no registered users found, "
                "connection closed";
            LOG(error_msg);
            perror(error_msg);
        }
        return user_id;
    }
    const char* error_msg =
        "Error: authentification failed, message of wrong type has been "
        "received, connection closed";
    LOG(error_msg);
    perror(error_msg);
    return std::nullopt;
};
