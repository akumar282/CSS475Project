#include "../inc/operation.h"

// command mappings
static bool isValidFlightNum(const std::string& flightNum) {
    const std::regex validFlightNumber("[A-Z]{2}[0-9]{3,4}");
    return std::regex_match(flightNum, validFlightNumber);
}
static bool isValidDateTime(const std::string& time) {
    const std::regex validDateTime("[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}");
    return std::regex_match(time, validDateTime);
}
static bool isValidICAO(const std::string& icao) {
    const std::regex validICAO("[A-Z]{4}");
    return std::regex_match(icao, validICAO);
}
static bool isValidGate(const std::string& gate) {
    const std::regex validGate("[A-Z][0-9]+");
    return std::regex_match(gate, validGate);
}
static bool isValidAirplane(const std::string& airplane) {
    const std::regex validAirplane("[a-zA-Z0-9 ]+");
    return std::regex_match(airplane, validAirplane);
}
static bool isValidAirline(const std::string& airline) {
    const std::regex validAirline("[a-zA-Z ]+");
    return std::regex_match(airline, validAirline);
}
// maps keyword to invoke command to its corresponding id
const std::map<std::string, operation_t> Operation::commandList = {
    {"exit", Operation::c_exit},
    {"e", Operation::c_exit},
    {"help", Operation::c_help},
    {"h", Operation::c_help},
    {"status", Operation::c_status},
    {"create", Operation::c_create},
    {"list_from_dest", Operation::c_list_from_dest}
};

//maps keyword to its corresponding help message
const std::map<std::string, std::string> Operation::commandHelp = {
    {"exit", "exit - exits program"},
    {"help", "help - lists all commands"},
    {"status", "status <flight-number> - gets the status of a flight"},
    {"list_from_dest", "list_from_dest "}

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
    if(args.empty()) return Error::BADARGS;
    std::string flightNum = args.front();
    if(!isValidFlightNum(flightNum)) return Error::BADARGS;
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


error_t Operation::list_from_dest(const API& api, const std::list<std::string>& args) {
    if(args.empty()) return Error::BADARGS;
    std::string flightNum = args.front();
    if(!isValidFlightNum(flightNum)) return Error::BADARGS;
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
// args = {flight-number, departure, arrival, gate, airplane, destination(ICAO), origin(ICAO), airline}
//
error_t Operation::create(const API& api, const std::list<std::string>& args) {
    // command has args

    if(args.empty()) {return Error::BADARGS;}
    auto it = args.begin();
    
    std::string flightNum = *it;
    if(!isValidFlightNum(flightNum)) {return Error::BADARGS;}
    std::string departure = *(++it);
    departure += " " + *(++it);
    std::string arrival = *(++it);
    arrival += " " + *(++it);
    if(!isValidDateTime(departure) || !isValidDateTime(arrival)) {return Error::BADARGS;}

    std::string gate = *(++it);
    if(!isValidGate(gate)) {return Error::BADARGS;}
    std::string airplane = *(++it);
    airplane += " " + *(++it);
    if(!isValidAirplane(airplane)) {return Error::BADARGS;}
    std::string destination = *(++it);
    std::string origin = *(++it); 
    if(!isValidICAO(destination) || !isValidICAO(origin)) {return Error::BADARGS;}
    std::string airline = *(++it);
    airline += " " + *(++it);
    if(!isValidAirline(airline)) {return Error::BADARGS;}
    auto Terminal = gate.substr(0, 1);
    auto gateNum = gate.substr(1, gate.length()-1);
    pqxx::connection connection = api.begin();
    pqxx::work query(connection);

    std::string CreateQuery = 
    "INSERT INTO Flight(id, flight_number, departure_time, arrival_time, num_passengers, gate_id, status_id, airplane_id, destination_id, origin_id, airline_id) "
    "VALUES ((Select NEXTVAL('flight_id_seq')), \'"
    + flightNum + "\', \'"
    + departure + "\', \'"
    + arrival + "\',"
    "0 , "
    "(Select GateType.id from GateType " 
        "JOIN TerminalType on TerminalType.id = GateType.terminal_id "
        "WHERE TerminalType.letter = \'" + Terminal + "\' "
        "AND GateType.gate_number =" + gateNum + " ), "
    "1, "
    "(Select id from AirplaneType WHERE AirplaneType.name = \'" + airplane + "\' ), " 
    "(Select id from LocationType WHERE LocationType.icao = \'" + destination +  "\' ), "
    "(Select id from LocationType WHERE LocationType.icao = \'" + origin + "\' ), "
    "(Select id from AirlineType WHERE AirlineType.name = \'" + airline+ "\' ));  ";
    query.exec(CreateQuery);
    query.commit();

    return Error::SUCCESS;
}
