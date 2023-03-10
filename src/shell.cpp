#include "../inc/shell.h"

Shell::Shell() : running(true), api(login()) {}

void Shell::start() {
    while(this->running) {
        Command cmd = fetchCommand();
        error_t status = executeCommand(cmd);

        if(status == Error::EXIT) this->running = false;
        if(status != Error::SUCCESS) std::cerr << status;
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
            std::string arg;
            while(std::getline(ss, arg, ' ')) {
                if(arg == "\n" || arg == "") continue;
                args.push_back(arg);
            }
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