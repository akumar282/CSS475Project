#include "../inc/operation.h"

// arguement validation

static bool isValidFlightNum(const API& api, const std::string& flightNum) {
    const std::regex validFlightNumber("[A-Z]{2}[0-9]{2,4}");
    if (!std::regex_match(flightNum, validFlightNumber))
        return false;
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
    return result1.at(0).at(0).as<int>() == 1;
}
static bool isValidDateTime(const std::string& dateTime) {
    const std::regex validDateTime("[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}");
    return std::regex_match(dateTime, validDateTime);
}
static bool isValidTime(const std::string& time) {
    const std::regex validTime("^(?:[01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9](?:\\.[0-9]{1,3})?$");
    return std::regex_match(time, validTime);
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
static bool isValidCity(const std::string& city) {
    const std::regex validCity("[a-zA-Z ]+");
    return std::regex_match(city, validCity);
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
    {"list", Operation::c_list},
    {"delay", Operation::c_delay},
    {"mealTypes", Operation::c_mealTypes},
    {"meals", Operation::c_meals},
    {"changeStatus", Operation::c_changeStatus},
    {"changeDestination", Operation::c_changeDestination}
};

//maps keyword to its corresponding help message
const std::map<std::string, std::string> Operation::commandHelp = {
    {"exit", "exit - exits program"},
    {"help", "help - lists all commands"},
    {"status", "status <flight-number> - gets information about a flight"},
    {"depart", "depart <icao> - lists flights leaving to <icao>"},
    {"arrive", "arrive <icao> - lists flights leaving from <icao>"},
    {"passengers", "passengers <flight-number> <+/-n> - adds (+) or subtracts (-) \'n\' passengers from the flight"},
    {"list", "list - lists every active flight"},
    {"delay", "delay <flight-number> <\"hh:mm:ss\"> - lists every active flight"},
    {"meals", "meals <flight-number> - lists all the meals on a flight"},
    {"mealTypes", "mealTypes <flight-number> - lists all the categories of meals on a flight"},
    {"changeStatus", "changeStatus <flight-number> - updates the status of the flight "},
    {"changeDestination", "changeDestination <flight-number> - changes the current destination to new destination"},
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
    if(!isValidFlightNum(api, flightNum)) return Error::BADARGS;
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
    
    pqxx::row rows;
    try {
        rows = query.exec_prepared1("get_flight", flightNum);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return Error::DBERROR;
    }
    
    std::cout << "Flight " << rows[0] << " from " << rows[9] << " to " << rows[10] << " is " << rows[6] << '\n';
    std::cout << "Expected departure at " << rows[1] << " and arrives at " << rows[2] << '\n';
    std::cout << "Flight uses a(n) " << rows[7] << " with " << rows[8] << '\n';
    std::cout << "Flight will use gate " << rows[4] << rows[5] << " and has " << rows[3] << " passengers." << std::endl;

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
    if(!isValidFlightNum(api ,flightNum)) {return Error::BADARGS;}
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

    connection.prepare("CreateFlight",
    "INSERT INTO Flight(id, flight_number, departure_time, arrival_time, gate_id, status_id, airplane_id, destination_id, origin_id, airline_id) "
    "VALUES ((SELECT NEXTVAL('flight_id_seq')),"
        "$1 , " 
        "$2 , " 
        "$3 , "
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

    pqxx::result result;

    try
    {    
        auto result = query.exec_prepared("CreateFlight", flightNum, departure, arrival, terminal, gateNum, airplane, destination, origin, airline);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return Error::DBERROR;
    }
    
    if (result.affected_rows() == 0) {
        std::cerr << "Flight " << flightNum << " already exists." << std::endl;
        return Error::BADARGS;
    }
    std::cout << "Flight " << flightNum << " created." << std::endl; 
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

    pqxx::result rows;

    try
    {
        rows = query.exec_prepared("get_destinations", icao);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return Error::DBERROR;
    }
    
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

    pqxx::result rows;
    
    try
    {    
        rows = query.exec_prepared("get_arrivals", icao);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return Error::DBERROR;
    }

    for(auto it = rows.begin(); it != rows.end(); ++it) {
        std::cout << "Flight " << it[0].as<std::string>() << " from " << it[1].as<std::string>() << '\n';
    }
    std::cout.flush();  
    return Error::SUCCESS;
}

error_t Operation::addCargo(const API& api, const std::list<std::string>& args) {
    if(args.size() != 2) return Error::BADARGS;
    std::string flightNum = args.front();
    if(!isValidFlightNum(api, flightNum)) return Error::BADARGS;
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

    pqxx::result rows; 
    try
    {    
        rows = query.exec_prepared("add_cargo", cargo, flightNum);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return Error::DBERROR;
    }

    for (auto it = rows.begin(); it != rows.end(); ++it) {
        std::cout << it[0].as<std::string>() << '\n';
    }
    std::cout.flush();
    return Error::SUCCESS;
}

// Function: List all active flights in chronological order → returns list of flights in chronological order
// args = {flight-number, departure, arrival, gate, airplane, destination(ICAO), origin(ICAO), airline}
//
error_t Operation::list(const API& api) {
    // #TODO add rest of get plane info here:
    /*
    if(args.empty()) return Error::BADARGS;
    std::string flightNum = args.front();
    if(!isValidFlightNum(flightNum)) return Error::BADARGS;
    */
    
    // flight number was specified and is valid
    pqxx::connection connection = api.begin();
    pqxx::work query(connection);
    
    connection.prepare(
        "all_flights",
        "SELECT flight_number, departure_time, arrival_time, GateType.gate_number, TerminalType.letter, " 
        "StatusType.name, c1.name AS destination, c2.name AS origin, AirlineType.name "
        "FROM Flight "
            "JOIN StatusType ON (Flight.status_id = StatusType.id) "
            "JOIN GateType ON (Flight.gate_id = GateType.id) "
            "JOIN TerminalType ON (GateType.terminal_id = TerminalType.id) "
            "JOIN LocationType dest ON (dest.id = Flight.destination_id) "
            "JOIN LocationType origin ON (origin.id = Flight.origin_id) "
            "JOIN CityType c1 ON (dest.city_id = c1.id ) "
            "JOIN CityType c2 ON (origin.city_id = c2.id) "
            "JOIN AirlineType ON (Flight.airline_id = AirlineType.id) "
        "WHERE (StatusType.name NOT LIKE 'Arrived') "
        "ORDER BY departure_time "
        ";"
    );
    pqxx::result rows;
    try
    {    
        rows = query.exec_prepared("all_flights");
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return Error::DBERROR;
    }
    
    std::cout << std::right << std::setw(10) << "Flight #" 
          << std::right << std::setw(24) << "Departure Time" 
          << std::right << std::setw(24) << "Arrival Time" 
          << std::right << std::setw(8) << "Gate" 
          << std::right << std::setw(11) << "Terminal" 
          << std::right << std::setw(14) << "Status" 
          << std::right << std::setw(19) << "Destination" 
          << std::right << std::setw(18) << "Origin" 
          << std::right << std::setw(20) << "Airline" 
          << '\n';

    std::cout << "----------------------------------------------------------------------------------------------------------------------------------------------------\n";

    
    for(auto it = rows.begin(); it != rows.end(); ++it) {
        std::cout   << std::right << std::setw(10) << it[0].as<std::string>()
                    << std::right << std::setw(24) << it[1].as<std::string>()
                    << std::right << std::setw(24) << it[2].as<std::string>()
                    << std::right << std::setw(8)  << it[3].as<std::string>()
                    << std::right << std::setw(11) << it[4].as<std::string>() 
                    << std::right << std::setw(14) << it[5].as<std::string>() 
                    << std::right << std::setw(19) << it[6].as<std::string>() 
                    << std::right << std::setw(18) << it[7].as<std::string>() 
                    << std::right << std::setw(20) << it[8].as<std::string>() 
                    << '\n';
    }
    std::cout.flush();  
    return Error::SUCCESS;
} 

error_t Operation::delay(const API& api, const std::list<std::string>& args) {
    if(args.empty()) return Error::BADARGS;

    auto it = args.begin();

    std::string flightNum = *it;
    if(!isValidFlightNum(api, flightNum)) return Error::BADARGS;
    std::string delay = *(++it);
    if(!isValidTime(delay)) return Error::BADARGS;

    pqxx::connection connection = api.begin();
    pqxx::work query(connection);

    connection.prepare(
        "delay_flight",
        "UPDATE flight "
        "SET " 
           "departure_time = ((SELECT departure_time FROM flight WHERE flight_number = $1)::TIMESTAMP + $2), " 
            "arrival_time = ((SELECT arrival_time FROM flight WHERE flight_number = $1)::TIMESTAMP + $2) "
        "WHERE flight_number = $1"
        ";"
    );

    pqxx::result result;
    try
    {
        result = query.exec_prepared("delay_flight", flightNum, delay);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return Error::DBERROR;
    }

    if(result.affected_rows() != 1) return Error::DBERROR;  
    query.commit();

    std::cout << "Flight " << flightNum << " delayed by " << delay << std::endl;
    return Error::SUCCESS;
}

error_t Operation::meals(const API& api, const std::list<std::string>& args) {
    if(args.empty()) return Error::BADARGS;

    std::string flightNum = args.front();
    if(!isValidFlightNum(api, flightNum)) return Error::BADARGS;

    pqxx::connection connection = api.begin();
    pqxx::work query(connection);

    connection.prepare(
        "getMeals",
        "SELECT MealType.name "
        "FROM MealToFlight "
            "JOIN MealType ON (MealToFlight.meal_id = MealType.id) "
            "JOIN StatusType ON (StatusType.id = MealToFlight.flight_id) "
        "WHERE MealToFlight.flight_id = (SELECT id FROM flight WHERE flight_number = $1) "
            "AND " 
            "("
                "StatusType.name NOT LIKE 'Arrived'"
                    "AND StatusType.name NOT LIKE 'Cancelled'"
            ")"
        "ORDER BY MealType.name"
        ";"
    );

    pqxx::result rows;
    try
    {
        rows = query.exec_prepared("getMeals", flightNum);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return Error::DBERROR;
    }

    for (auto it = rows.begin(); it != rows.end(); ++it) {
        std::cout << it[0].as<std::string>() << '\n';
    }
    std::cout.flush();
    return Error::SUCCESS;

}

error_t Operation::checkCargo(const API& api, const std::list<std::string>& args) {
    if(args.size() != 2) return Error::BADARGS;
    std::string flightNum = args.front();
    if(!isValidFlightNum(api, flightNum)) return Error::BADARGS;
    std::string cargo = *(++args.begin());
    
    // flight number was specified and is valid
    pqxx::connection connection = api.begin();
    pqxx::work query(connection);
    
    connection.prepare(
        "check_cargo",
        "SELECT SUM(weight_lb) FROM Cargo "
        "WHERE flight_id = (SELECT id FROM Flight WHERE flight_number = $1)"
        ";"
    );

    pqxx::result rows;
    try
    {
        rows = query.exec_prepared("check_cargo", flightNum);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return Error::DBERROR;
    }

    for (auto it = rows.begin(); it != rows.end(); ++it) {
        std::cout << it[0].as<std::string>() << '\n';
    }
    std::cout.flush();
    return Error::SUCCESS;
}
error_t Operation::mealsOffered(const API& api, const std::list<std::string>& args) {
    if(args.empty()) return Error::BADARGS;
    std::string flightNum = args.front();
    //checks for valid flight number and if its a duplicate
    if(!isValidFlightNum(api, flightNum)) return Error::BADARGS; 
    pqxx::connection connection = api.begin();
    pqxx::work query(connection);

    connection.prepare(
    "CheckMealType",
    "SELECT Distinct MealCategoryType.category "
    "FROM MealCategoryType "
        "JOIN MealToCategory ON (MealCategoryType.id = MealToCategory.category_id) "
        "JOIN MealType ON (MealToCategory.meal_id = MealType.id) "
        "JOIN MealToFlight ON (MealType.id = MealToFlight.meal_id) "
    "WHERE MealToFlight.flight_id = (SELECT Flight.id FROM Flight WHERE Flight.flight_number = $1) "
    "ORDER BY MealCategoryType.category; "
    );

    pqxx::result rows;
    try
    {
        rows = query.exec_prepared("CheckMealType", flightNum);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return Error::DBERROR;
    }
    query.commit();

    for (auto it = rows.begin(); it != rows.end(); ++it) {
        std::cout << it[0].as<std::string>() << '\n';
    }
    return Error::SUCCESS;
}


error_t Operation::passengers(const API& api, const std::list<std::string>& args) {
    if (args.size() != 2) return Error::BADARGS;

    auto it = args.begin();

    std::string flightNum = *(it);
    if (!isValidFlightNum(api, flightNum)) return Error::BADARGS;
    
    std::string numPassengers = *(++it);

    pqxx::connection connection = api.begin();
    pqxx::work query(connection);

    connection.prepare(
        "update_passengers",
        "UPDATE Flight "
        "SET num_passengers = num_passengers + $1 "
        "WHERE flight_number = $2 AND (num_passengers + $1) > 0 AND (num_passengers + $1) < (SELECT max_passengers FROM AirplaneType WHERE id = Flight.airplane_id); "
    );

    connection.prepare(
        "get_passengers",
        "SELECT num_passengers FROM flight "
        "WHERE flight_number = $1"
        ";"
    );

    pqxx::result rows;
    try
    {
        rows = query.exec_prepared("update_passengers", numPassengers, flightNum);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return Error::DBERROR;
    }

    if(rows.affected_rows() != 1) {
        std::cout << "Error: Could not update passengers." << std::endl;
        return Error::DBERROR;
    }

    query.commit();

    try
    {    
        rows = query.exec_prepared("get_passengers", flightNum);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return Error::DBERROR;
    }
    
    for(auto it = rows.begin(); it != rows.end(); ++it) {
         std::cout << "Flight now has " << it[0].as<std::string>() << " passengers." << std::endl;
    }

    return Error::SUCCESS;
}


error_t Operation::changeStatus(const API& api, const std::list<std::string>& args) {
    if (args.empty()) return Error::BADARGS;

    auto it = args.begin();

    std::string flightNum = *(it);
    if (!isValidFlightNum(api, flightNum)) return Error::BADARGS;
    
    std::string newStatus = *(++it);
    if (!std::regex_match(newStatus, std::regex("^(Standby|Boarding|Departed|Delayed|In Transit|Arrived|Cancelled)$"))) return Error::BADARGS;
    
    std::cout << "flight number: " << flightNum << std::endl;
    std::cout << "new status: " << newStatus << std::endl;


    pqxx::connection connection = api.begin();
    pqxx::work query(connection);

    connection.prepare(
        "update_status",
        "UPDATE Flight "
        "SET status_id = (SELECT id FROM StatusType WHERE name = $1) "
        "WHERE flight_number = $2; "
    );

    connection.prepare(
        "get_status",
        "SELECT StatusType.name FROM Flight "
            "JOIN StatusType ON (Flight.status_id = StatusType.id)  "
        "WHERE flight_number = $1; "
    );

    pqxx::result rows;
    try
    {
        rows = query.exec_prepared("update_status", newStatus, flightNum);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return Error::DBERROR;
    }

    query.commit();

    try
    {    
        rows = query.exec_prepared("get_status", flightNum);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return Error::DBERROR;
    }
    
    for(auto it = rows.begin(); it != rows.end(); ++it) {
         std::cout << "Flight now has a status " << it[0].as<std::string>() << std::endl;
    }

    return Error::SUCCESS;
}



// Set destination: Update the destination
//          Edge 1: Can't be cancelled, arrived, or in the air
//          Edge 2: The origin needs to be our airport 
//          
error_t Operation::changeDestination(const API& api, const std::list<std::string>& args) {
    if (args.empty()) return Error::BADARGS;

    auto it = args.begin();

    std::string flightNum = *(it);
    if (!isValidFlightNum(api, flightNum)) return Error::BADARGS;
    
    std::string newDestination = *(++it);
    if (!isValidCity(newDestination)) return Error::BADARGS;
    
    pqxx::connection connection = api.begin();
    pqxx::work query(connection);

    connection.prepare(
        "update_destination",
        "UPDATE Flight "
        "SET destination_id =   (SELECT LocationType.id "
                                "FROM LocationType "
                                    "JOIN CityType ON (CityType.id = LocationType.city_id) "
                                "WHERE CityType.name = $1 AND CityType.name NOT LIKE 'Detroit') "
        "WHERE flight_number = $2 "
        "AND origin_id = ( "
            "SELECT LocationType.id "
            "FROM LocationType "
                "JOIN CityType ON (CityType.id = LocationType.city_id) "
            "WHERE CityType.name = 'Detroit') "
        "AND status_id = ( "
            "SELECT id "
            "FROM StatusType "
            "WHERE StatusType.name IN ('Standby', 'Delayed') "
        "); "
                                                                          
    );

    connection.prepare(
        "get_destination",
        "SELECT CityType.name FROM Flight "
            "JOIN LocationType ON (Flight.destination_id = LocationType.id)  "
            "JOIN CityType ON (LocationType.city_id = CityType.id)  "
        "WHERE flight_number = $1; "
    );

    pqxx::result rows;
    try
    {
        rows = query.exec_prepared("update_destination", newDestination, flightNum);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return Error::DBERROR;
    }

    query.commit();

    try
    {    
        rows = query.exec_prepared("get_destination", flightNum);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return Error::DBERROR;
    }
    
    for(auto it = rows.begin(); it != rows.end(); ++it) {
         std::cout << "The new destination for the flight is " << it[0].as<std::string>() << std::endl;
    }

    return Error::SUCCESS;
}