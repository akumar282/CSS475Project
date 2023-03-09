#include "../inc/command.h"



// constructors
Command::Command(){};

Command::Command(const std::string& command, const std::list<std::string> args) 
    : command(command), args(args) {
}

Command::Command(const Command& c) 
    : command(c.command), args(c.args) {
}

// ostream for debugging
std::ostream& operator<<(std::ostream& os, const Command& c) {
    os << std::setw(12) << c.getCommand() << ' ';
    for(const auto& arg : c.getArgs()) {
        os << arg << ' ';
    };
    return os;
}

// getters
std::string Command::getCommand() const {
    return this->command;
}
std::list<std::string> Command::getArgs() const {
    return this->args;
}
