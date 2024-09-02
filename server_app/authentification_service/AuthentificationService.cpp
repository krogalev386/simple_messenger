#include "AuthentificationService.hpp"

#include <algorithm>
#include <cstring>

AuthentificationService::AuthentificationService()
{
    auto user_1 = std::pair<UserID, UserCredentials>(1, UserCredentials{"client_a", "client_a"});
    auto user_2 = std::pair<UserID, UserCredentials>(2, UserCredentials{"client_b", "client_b"});
    registered_users.push_back(user_1);
    registered_users.push_back(user_2);
};

bool AuthentificationService::checkIfRegistered(const UserCredentials& considered_cred)
{
    auto record = std::find_if(registered_users.begin(),
                               registered_users.end(),
                               [&considered_cred](const std::pair<UserID, UserCredentials>& cred)
                                   {
                                       return (strcmp(cred.second.nickname, considered_cred.nickname) == 0)
                                          and (strcmp(cred.second.password, considered_cred.password) == 0);
                                   }
                               );

    return record != registered_users.end();
}