#include "../inc/shell.h"

Shell::Shell() : running(true), api(login()) {}

void Shell::start() {
    while(this->running) {
        Command cmd = fetchCommand();
        error_t status = executeCommand(cmd);

        if(status == Error::EXIT) {
            this->running = false;
            continue;
        }
        if(status == Error::BADARGS) {
            std::cerr << "Bad Arguments" << std::endl;
            continue;
        }
        if( status == Error::DBERROR) {
            std::cerr << "Database Error" << std::endl;
            continue;
        }
        if(status != Error::SUCCESS) std::cerr << status; //keep this at the bottom
    }
}

Command Shell::fetchCommand() {

    bool validCommand = false;

    while(!validCommand) {
        std::string input;
        std::cout << "air>";
        std::cout.flush();
        std::getline(std::cin, input);

        std::stringstream ss(input);

        // fetch first token (will be command)
        std::string command;
        ss >> command; 

        // valid command
        if(Operation::commandList.find(command) != Operation::commandList.end()) {
            std::list<std::string> args;
            // create AA1234 "2021-03-01 12:00:00" "2021-03-01 14:00:00" A3 "Boeing 787" KLAX KJFK "American Airlines"

            std::regex re("\"[^\"]*\"|\\S+");

            std::sregex_iterator it(input.begin(), input.end(), re);
            std::sregex_iterator end;

            while (it != end) {
                std::string str = it->str();
                if (str.front() == '"' && str.back() == '"') {
                    str = str.substr(1, str.length() - 2);
                }
                args.push_back(str);
                ++it;
            }
            args.pop_front();
            return Command(command, args);
        }
        // invalid command
        else {
            std::cout << "Invalid Command\n";
            ss.clear();
        }
    }

    throw new std::logic_error("Invalid State.");
    return Command();
}

error_t Shell::executeCommand(const Command& c) {
    switch(Operation::commandList.at(c.getCommand())) {
    case Operation::c_exit : {
        return Operation::shell_exit();
    }
    case Operation::c_help : { 
        return Operation::help();
    }
    case Operation::c_status : { 
        return Operation::status(this->getAPI(), c.getArgs());
    }
    case Operation::c_create : {
        return Operation::create(this->getAPI(), c.getArgs());
    }
    case Operation::c_depart : {
        return Operation::depart(this->getAPI(), c.getArgs());
    }
    case Operation::c_arrive : {
        return Operation::arrive(this->getAPI(), c.getArgs());
    }
    case Operation::c_passengers : {
        return Operation::passengers(this->getAPI(), c.getArgs());
    }
    default : {
        return Error::BADCMD;
    }
    }
}

API Shell::login() {
    std::string username, password;
    std::cout << "Enter username:";
    std::cin >> username;
    std::cout << "Enter password:";
    std::cin >> password;
    // empty \n character
    std::cin.ignore();
    return API(username, password);
}

const API& Shell::getAPI() { return this->api; }