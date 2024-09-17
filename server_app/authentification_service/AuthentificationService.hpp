#pragma once

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
    bool checkIfRegistered(const UserCredentials&);

   private:
    std::vector<std::pair<UserID, UserCredentials>> registered_users;
};
