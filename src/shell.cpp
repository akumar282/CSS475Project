#include "../inc/shell.h"

const std::map<std::string, int> Shell::commands = {
    {"help", 0},
    {"h", 0}
};

Shell::Shell() {
    this->running = true;
}

void Shell::start() {
    while(this->running) {
        Command cmd = fetchCommand();
    }
}

const Command& Shell::fetchCommand() {

    bool validCommand = false;

    while(!validCommand) {
        std::string input;
        std::cout << "air>" << std::endl;
        std::getline(std::cin, input);

        std::streamstream ss(input);

        // fetch first token (will be command)
        std::string command;
        ss >> command; 

        // valid command
        if(Command::commands.find(command) != Command::commands.end()) {

        }
        else {
            ss.clear();
        }
    }
}