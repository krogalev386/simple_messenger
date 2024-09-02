#pragma once

#include <utility>
#include <vector>

#include "CrtpSingleton.hpp"
#include "defines.hpp"

class AuthentificationService : public CrtpSingleton<AuthentificationService> {
    friend class CrtpSingleton<AuthentificationService>;

   protected:
    AuthentificationService();
    ~AuthentificationService() = default;

   public:
    bool checkIfRegistered(const UserCredentials&);

   private:
    std::vector<std::pair<UserID, UserCredentials>> registered_users;
};
