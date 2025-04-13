#pragma once

#include <optional>
#include <utility>
#include <vector>

#include "DynamicCrtpSingleton.hpp"
#include "defines.hpp"

class AuthentificationService
    : public DynamicCrtpSingleton<AuthentificationService> {
    friend class DynamicCrtpSingleton<AuthentificationService>;

   protected:
    AuthentificationService();
    ~AuthentificationService() = default;

   public:
    static std::optional<UserID> checkIfRegistered(const UserCredentials&);

   public:
    static std::optional<UserID> handleAuthReq(const Envelope&);

    // private:
    //  std::vector<std::pair<UserID, UserCredentials>> registered_users;
};
