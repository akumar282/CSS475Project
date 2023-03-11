#include "../inc/operation.h"

// command mappings

// maps keyword to invoke command to its corresponding id
const std::map<std::string, operation_t> Operation::commandList = {
    {"exit", Operation::c_exit},
    {"e", Operation::c_exit},
    {"help", Operation::c_help},
    {"h", Operation::c_help},
    {"status", Operation::c_status},
    {"create", Operation::c_create}
};

//maps keyword to its corresponding help message
const std::map<std::string, std::string> Operation::commandHelp = {
    {"exit", "exit - exits program"},
    {"help", "help - lists all commands"},
    {"status", "status <flight-number> - gets the status of a flight"},
};

// command implementation

error_t Operation::help() {
    // command has no args
    for(auto& [cmd, id] : Operation::commandList) {
        if(Operation::commandHelp.find(cmd) != Operation::commandHelp.end()) {
            // help message exists for command
            std::cout << Operation::commandHelp.at(cmd) << '\n';
        }
    }
    return Error::SUCCESS;
}

error_t Operation::shell_exit() {
    return Error::EXIT;
}

error_t Operation::status(const API& api, const std::list<std::string>& args) {
    const std::regex validFlightNumber("[A-Z]{2}[0-9]{3,4}");
    if(args.empty()) return Error::BADARGS;
    std::string flightNum = args.front();
    if(!std::regex_match(flightNum, validFlightNumber)) return Error::BADARGS;

    // flight number was specified and is valid
    pqxx::connection connection = api.begin();
    pqxx::work query(connection);
    
    // we could abstract this out; not sure
    std::string queryString = 
    "SELECT name "
    "FROM flight "
        "JOIN statustype ON (flight.status_id = statustype.id) "
    "WHERE flight_number = \'"
    + flightNum + "\' ;";
    pqxx::row row;
    try {  
        row = query.exec1(queryString);
    }
    catch(const std::exception& e) {
        // could not find
    }

    std::cout << row.at(0).as<std::string>() << std::endl;

    return Error::SUCCESS;
}   

// Inside of args
//
//
//
error_t Operation::create(std::list<std::string> args) {
    // command has args
    if(args.empty()) {return Error::BADARGS;}

    // Creating a flight
}
