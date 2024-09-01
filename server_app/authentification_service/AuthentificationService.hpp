#pragma once

#include "CrtpSingleton.hpp"
#include "defines.hpp"

#include <vector>
#include <utility>

class AuthentificationService : public CrtpSingleton<AuthentificationService>
{
friend class CrtpSingleton<AuthentificationService>;

protected:
    AuthentificationService();
    ~AuthentificationService() = default;
    
public:
    bool checkIfRegistered(const UserCredentials&);

private:
    std::vector<std::pair<uint64_t, UserCredentials>> registered_users;
};
