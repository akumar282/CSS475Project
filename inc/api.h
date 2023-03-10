#pragma once

#include <iostream>
#include <pqxx/pqxx>

class API {

private:

    // these will remain constant
    static const std::string host;
    static const std::string port;
    static const std::string dbname;
    static const std::string connect_timeout;
    // user and password can change
    std::string user;
    std::string password;

    std::string getConnectionString() const;

public:

    API(std::string, std::string);
    API(const API&);

    pqxx::connection begin() const;

};

