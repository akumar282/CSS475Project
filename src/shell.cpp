#include "../inc/shell.h"

Shell::Shell() {
    this->running = true;
}

void Shell::start() {
    while(this->running) {
        Command cmd = fetchCommand();
        std::cout << cmd << std::endl;
    }
}

const Command& Shell::fetchCommand() {

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
        if(Command::commands.find(command) != Command::commands.end()) {
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