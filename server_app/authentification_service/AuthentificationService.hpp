#pragma once

#include "defines.hpp"
#include <vector>
#include <utility>

class AuthentificationService
{
private:
    AuthentificationService()  = default;
    ~AuthentificationService() = default;

    AuthentificationService(const AuthentificationService&) = delete;
    AuthentificationService(AuthentificationService&&)      = delete;

public:
    static AuthentificationService& getInstance()
    {
        static AuthentificationService instance;
        return instance;
    }

    void init();
    bool checkIfRegistered(const UserCredentials&);

private:
    std::vector<std::pair<uint64_t, UserCredentials>> registered_users;
};
