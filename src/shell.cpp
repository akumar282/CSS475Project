#include "../inc/shell.h"

Shell::Shell() {
    this->running = true;
}

void Shell::start() {
    while(this->running) {
        Command cmd = fetchCommand();
        error_t status = executeCommand(cmd);

        if(status == Error::EXIT) this->running = false;
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
    default : {
        return Error::BADCMD;
    }
    }
}