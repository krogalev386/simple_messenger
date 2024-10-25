#include "AuthentificationService.hpp"

#include <algorithm>
#include <cstring>
#include <optional>

#include "PsqlManager.hpp"
#include "defines.hpp"

AuthentificationService::AuthentificationService() = default;
;

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