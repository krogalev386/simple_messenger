#pragma once

#include <libpq-fe.h>

#include <defines.hpp>
#include <optional>

#include "StaticCrtpSingleton.hpp"

class PsqlManager : public StaticCrtpSingleton<PsqlManager> {
    friend class StaticCrtpSingleton<PsqlManager>;

   private:
    PGconn*   pg_connection;
    PGresult* pg_result;
    bool      is_connected;

   protected:
    PsqlManager() : is_connected(false){};
    ~PsqlManager() {
        if (isConnected()) {
            closeConnection();
        }
    };

   public:
    bool isConnected() const { return is_connected; };

    bool tryConnect(const char* username, const char* password,
                    const char* hostaddr, const char* port);

    bool createUsersTableIfNotExist();

    std::optional<UserID>        registerNewUser(const char* name,
                                                 const char* password,
                                                 const char* email);
    static std::optional<UserID> removeUser(const UserID user_id);
    static std::optional<UserID> findUserByEmail(const char* email);

    static std::optional<UserCredentials> retriveUserCredsByID(const UserID);

    void closeConnection();
};