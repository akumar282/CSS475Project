#include "../inc/command.h"

const std::map<std::string, int> Command::commands = {
    {"exit", 0},
    {"e", 0},
    {"help", 1},
    {"h", 1}
};

Command::Command(){};

Command::Command(const std::string& command, const std::list<std::string> args) 
    : command(command), args(args) {
    this->commandId = Command::commands.at(command);
}

Command::Command(const Command& c) 
    : commandId(c.commandId), command(c.command), args(c.args) {
}

std::ostream& operator<<(std::ostream& os, const Command& c) {
    os << c.getCommand() << ' ';
    for(const auto& arg : c.getArgs()) {
        os << arg << ' ';
    };
    return os;
}

int Command::getCommandId() const {
    return this->commandId;
}

std::string Command::getCommand() const {
    return this->command;
}
std::list<std::string> Command::getArgs() const {
    return this->args;
}
