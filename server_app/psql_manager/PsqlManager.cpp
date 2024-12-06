#include <libpq-fe.h>

#include <Logger.hpp>
#include <PsqlManager.hpp>
#include <defines.hpp>
#include <optional>

bool PsqlManager::tryConnect(const char* username, const char* password,
                             const char* hostaddr, const char* port) {
    std::string connstr =
        "dbname=" + std::string("postgres") + " user=" + std::string(username) +
        " password=" + std::string(password) +
        " hostaddr=" + std::string(hostaddr) + " port=" + std::string(port);
    LOG("Attempt to connect to database...");
    printf("%s\n", connstr.data());
    pg_connection = PQconnectdb(connstr.data());

    if (PQstatus(pg_connection) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n",
                PQerrorMessage(pg_connection));
        LOG("Connection to database failed: %s", PQerrorMessage(pg_connection));
        PQfinish(pg_connection);
        is_connected = false;
        return false;
    }
    is_connected = true;
    printf("Connected to database successfully.\n");
    return true;
};

bool PsqlManager::createUsersTableIfNotExist() {
    // Create table SQL statement
    const char* create_table_sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id SERIAL PRIMARY KEY, "
        "name VARCHAR(100) NOT NULL, "
        "password VARCHAR(50) NOT NULL, "
        "email VARCHAR(255) NOT NULL UNIQUE)";
    // Execute the CREATE TABLE query
    pg_result = PQexec(pg_connection, create_table_sql);
    LOG("Attempt to create new table...");

    if (PQresultStatus(pg_result) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error creating table: %s\n",
                PQerrorMessage(pg_connection));
        LOG("Error creating table: %s", PQerrorMessage(pg_connection));
        PQclear(pg_result);
        return false;
    }
    printf("Table created successfully.\n");
    return true;
};

std::optional<UserID> PsqlManager::registerNewUser(const char* name,
                                                   const char* password,
                                                   const char* email) {
    std::string insert_req =
        "INSERT INTO users (name, password, email) VALUES ('" +
        std::string(name) + "', '" + std::string(password) + "', '" +
        std::string(email) + "') RETURNING id";

    // Prepare and execute the INSERT query
    pg_result = PQexec(pg_connection, insert_req.data());

    if (PQresultStatus(pg_result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error inserting data: %s\n",
                PQerrorMessage(pg_connection));
        PQclear(pg_result);
        return std::nullopt;
    }
    printf("Data inserted successfully.\n");
    auto new_user_id =
        static_cast<UserID>(std::atoi(PQgetvalue(pg_result, 0, 0)));
    PQclear(pg_result);
    return new_user_id;
};

std::optional<UserID> PsqlManager::removeUser(const UserID /*user_id*/) {
    // TO DO;
    return std::nullopt;
};

std::optional<UserID> PsqlManager::findUserByCreds(
    const UserCredentials& creds) {
    std::string select_req =
        "SELECT id FROM users WHERE email='" + std::string(creds.email) + "'" +
        " AND password='" + std::string(creds.password) + "'";

    pg_result = PQexec(pg_connection, select_req.data());

    if (PQresultStatus(pg_result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error requesting data: %s\n",
                PQerrorMessage(pg_connection));
        LOG("Error requesting data: %s\n", PQerrorMessage(pg_connection));
        PQclear(pg_result);
        return std::nullopt;
    }
    if (PQntuples(pg_result) < 1) {
        fprintf(stderr, "Error requesting data: No user found\n");
        LOG("Error requesting data: No user found\n");
        PQclear(pg_result);
        return std::nullopt;
    }
    auto user_id = static_cast<UserID>(std::atoi(PQgetvalue(pg_result, 0, 0)));
    PQclear(pg_result);
    printf("User %lu found", user_id);
    LOG("User %lu found", user_id);
    return user_id;
};

void PsqlManager::closeConnection() {
    // Close the connection
    PQfinish(pg_connection);
};
