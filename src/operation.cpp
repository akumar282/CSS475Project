#include "../inc/operation.h"

// arguement validation

static bool isValidFlightNum(const std::string& flightNum) {
    const std::regex validFlightNumber("[A-Z]{2}[0-9]{2,4}");
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

// command mappings

// maps keyword to invoke command to its corresponding id
const std::map<std::string, operation_t> Operation::commandList = {
    {"exit", Operation::c_exit},
    {"help", Operation::c_help},
    {"status", Operation::c_status},
    {"create", Operation::c_create},
    {"depart", Operation::c_depart},
    {"arrive", Operation::c_arrive},
    {"passengers", Operation::c_passengers},
};

//maps keyword to its corresponding help message
const std::map<std::string, std::string> Operation::commandHelp = {
    {"exit", "exit - exits program"},
    {"help", "help - lists all commands"},
    {"status", "status <flight-number> - gets information about a flight"},
    {"depart", "depart <icao> - lists flights leaving from <icao>"},
    {"arrive", "arrive <icao> - lists flights leaving from <icao>"},
    {"passengers", "passengers <flight-number> - lists number of passengers on the plane"}
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
    // #TODO add rest of get plane info here:
    if(args.empty()) return Error::BADARGS;
    std::string flightNum = args.front();
    if(!isValidFlightNum(flightNum)) return Error::BADARGS;
    // flight number was specified and is valid
    pqxx::connection connection = api.begin();
    pqxx::work query(connection);
    
    // we could abstract this out; not sure
    connection.prepare(
    "get_flight",
    "SELECT flight_number, departure_time, arrival_time, num_passengers, letter, gate_number, statustype.name, airplanetype.name, airlinetype.name, origin.icao, destination.icao " 
    "FROM flight " 
        "JOIN gatetype ON (flight.gate_id = gatetype.id) "
        "JOIN terminaltype ON (gatetype.terminal_id = terminaltype.id)"
        "JOIN statustype ON (flight.status_id = statustype.id) "
        "JOIN airplanetype ON (flight.airplane_id = airplanetype.id) "
        "JOIN airlinetype ON (flight.airline_id = airlinetype.id) "
        "JOIN locationtype AS origin ON (flight.origin_id = origin.id) "
        "JOIN locationtype AS destination ON (flight.destination_id = destination.id) "
    "WHERE flight_number = $1 "
        "AND" 
    "("
        "StatusType.name NOT LIKE 'Arrived'"
            "AND StatusType.name NOT LIKE 'Cancelled'"
    ")"
    );
    // flight_number, departure_time, arrival_time, num_passengers, letter, gate_number, statustype.name, airplanetype.name, airlinetype.name, origin.icao, destination.icao
    // 0              1               2             3               4       5            6                7                  8                 9            10
    auto rows = query.exec_prepared1("get_flight", flightNum);
    std::cout << "Flight " << rows[0] << " from " << rows[9] << " to " << rows[10] << " is " << rows[6] << '\n';
    std::cout << "Expected departure at " << rows[1] << " and arrives at " << rows[2] << '\n';
    std::cout << "Flight uses a(n) " << rows[7] << " with " << rows[8] << '\n';  

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
    std::string arrival = *(++it);
    if(!isValidDateTime(departure) || !isValidDateTime(arrival)) {return Error::BADARGS;}
    std::string gate = *(++it);
    if(!isValidGate(gate)) {return Error::BADARGS;}
    std::string airplane = *(++it);
    if(!isValidAirplane(airplane)) {return Error::BADARGS;}
    std::string destination = *(++it);
    std::string origin = *(++it); 
    if(!isValidICAO(destination) || !isValidICAO(origin)) {return Error::BADARGS;}
    std::string airline = *(++it);
    if(!isValidAirline(airline)) {return Error::BADARGS;}
    auto terminal = gate.substr(0, 1);
    auto gateNum = gate.substr(1, gate.length()-1);
    pqxx::connection connection = api.begin();
    pqxx::work query(connection);

    connection.prepare("CheckDup",
    "SELECT COUNT(*) "
    "FROM Flight "
    "JOIN StatusType ON (Flight.status_id = StatusType.id) "
    "WHERE (StatusType.name NOT LIKE 'Arrived' "
        "AND StatusType.name NOT LIKE 'Cancelled') "
    "AND flight_number = $1 ; "
    );
    pqxx::result result1 = query.exec_prepared("CheckDup", flightNum);
    if(result1.at(0).at(0).as<int>() != 0) {
        // duplicate flight number
        return Error::DBERROR;
    }
    connection.prepare("CreateFlight",
    "INSERT INTO Flight(id, flight_number, departure_time, arrival_time, num_passengers, gate_id, status_id, airplane_id, destination_id, origin_id, airline_id) "
    "VALUES ((SELECT NEXTVAL('flight_id_seq')),"
        "$1 , " 
        "$2 , " 
        "$3 , "
        "0 , "
        "(SELECT GateType.id FROM GateType " 
            "JOIN TerminalType ON (TerminalType.id = GateType.terminal_id) "
            "WHERE TerminalType.letter = $4 "
                "AND GateType.gate_number = $5 ), "
        "1, "
        "(SELECT id FROM AirplaneType WHERE AirplaneType.name = $6 ), " 
        "(SELECT id FROM LocationType WHERE LocationType.icao = $7 ), "
        "(SELECT id FROM LocationType WHERE LocationType.icao = $8 ), "
        "(SELECT id FROM AirlineType WHERE AirlineType.name = $9 ));  "
    );
    auto result = query.exec_prepared("CreateFlight", flightNum, departure, arrival, terminal, gateNum, airplane, destination, origin, airline);
    if (result.affected_rows() == 0) {
        return Error::BADARGS;
    }
    query.commit();

    return Error::SUCCESS;
}

error_t Operation::depart(const API& api, const std::list<std::string>& args) {
    std::string icao = args.front();
    if(!isValidICAO(icao)) { return Error::BADARGS; }

    pqxx::connection connection = api.begin();
    pqxx::work query(connection);

    connection.prepare(
        "get_destinations",
        "SELECT flight_number, destination.icao FROM flight "
            "JOIN LocationType AS origin ON (flight.origin_id = origin.id) "
            "JOIN LocationType AS destination ON (flight.destination_id = destination.id) "
            "JOIN StatusType ON (flight.status_id = StatusType.id) "
        "WHERE origin.icao = $1 "
            "AND " 
            "("
                "StatusType.name NOT LIKE 'Arrived'"
                    "AND StatusType.name NOT LIKE 'Cancelled'"
            ")"
        ";"
    );

    auto rows = query.exec_prepared("get_destinations", icao);

    for(auto it = rows.begin(); it != rows.end(); ++it) {
        std::cout << "Flight " << it[0].as<std::string>() << " to " << it[1].as<std::string>() << '\n';
    }
    std::cout.flush();  
    return Error::SUCCESS;
}

error_t Operation::arrive(const API& api, const std::list<std::string>& args) {
    std::string icao = args.front();
    if(!isValidICAO(icao)) { return Error::BADARGS; }

    pqxx::connection connection = api.begin();
    pqxx::work query(connection);

    connection.prepare(
        "get_arrivals",
        "SELECT flight_number, origin.icao FROM flight "
            "JOIN LocationType AS origin ON (flight.origin_id = origin.id) "
            "JOIN LocationType AS destination ON (flight.destination_id = destination.id) "
            "JOIN StatusType ON (flight.status_id = StatusType.id) "
        "WHERE destination.icao = $1"
            "AND " 
            "("
                "StatusType.name NOT LIKE 'Arrived'"
                    "AND StatusType.name NOT LIKE 'Cancelled'"
            ")"
        ";"
    );

    auto rows = query.exec_prepared("get_arrivals", icao);

    for(auto it = rows.begin(); it != rows.end(); ++it) {
        std::cout << "Flight " << it[0].as<std::string>() << " from " << it[1].as<std::string>() << '\n';
    }
    std::cout.flush();  
    return Error::SUCCESS;
}

// Function: Return number of passengers on plane → returns the number of passengers
//
error_t Operation::passengers(const API& api, const std::list<std::string>& args) {
    // #TODO add rest of get plane info here:
    if(args.empty()) return Error::BADARGS;
    std::string flightNum = args.front();
    if(!isValidFlightNum(flightNum)) return Error::BADARGS;
    
    // flight number was specified and is valid
    pqxx::connection connection = api.begin();
    pqxx::work query(connection);
    
    connection.prepare(
        "total_passengers",
        "SELECT num_passengers FROM flight "
        "WHERE flight_number = $1"
        ";"
    );

    auto rows = query.exec_prepared("total_passengers", flightNum);
    
    for(auto it = rows.begin(); it != rows.end(); ++it) {
        std::cout << it[0].as<std::string>() << '\n';
    }
    std::cout.flush();  
    return Error::SUCCESS;
}

error_t Operation::addCargo(const API& api, const std::list<std::string>& args) {
    if(args.size() != 2) return Error::BADARGS;
    std::string flightNum = args.front();
    if(!isValidFlightNum(flightNum)) return Error::BADARGS;
    std::string cargo = *(++args.begin());
    
    // flight number was specified and is valid
    pqxx::connection connection = api.begin();
    pqxx::work query(connection);
    
    connection.prepare(
        "add_cargo",
        "INSERT INTO Cargo(id, flight_id, weight_lb)"
        "VALUES ((SELECT NEXTVAL('cargo_id_seq')),"
        "(SELECT id FROM Flight WHERE flight_number = $1),"
        "$2)"
      ";"
    );

    auto rows = query.exec_prepared("add_cargo", cargo, flightNum);

    for (auto it = rows.begin(); it != rows.end(); ++it) {
        std::cout << it[0].as<std::string>() << '\n';
    }
    std::cout.flush();
    return Error::SUCCESS;
}
