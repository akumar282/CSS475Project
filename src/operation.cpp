#include "../inc/operation.h"

// command mappings
const std::map<std::string, operation_t> Operation::commandList = {
    {"exit", Operation::c_exit},
    {"e", Operation::c_exit},
    {"help", Operation::c_help},
    {"h", Operation::c_help}
};
const std::map<std::string, std::string> Operation::commandHelp = {
    {"exit", "exit - exits program"},
    {"help", "help - lists all commands"}
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