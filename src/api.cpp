#include "../inc/api.h"

// default connections
const std::string API::host = "localhost";
const std::string API::port = "5433";
const std::string API::dbname = "airport";
const std::string API::connect_timeout = "1";

API::API(std::string user, std::string password) 
: user(user), password(password) {}

API::API(const API& api)
: user(api.user), password(api.password) {}

std::string API::getConnectionString() const {
    return "host=" + this->host + " port=" + this->port + " dbname=" 
    + this->dbname + " connect_timeout=" + this->connect_timeout + " user=" 
    + this->user + " password=" + this->password;
}

pqxx::connection API::begin() const {
    return pqxx::connection(this->getConnectionString());
}
