#pragma once

#include "command.h"
#include "error.h"

// defines operation ids for jump table
typedef int operation_t;

class Operation {
public:

    // operation ids
    static constexpr operation_t c_exit = 0;
    static constexpr operation_t c_help = 1;
    static constexpr operation_t c_create = 2;

    // operation functions
    static error_t shell_exit();
    static error_t help();
    static error_t create(std::list<std::string>);
    
    // mappings
    static const std::map<std::string, operation_t> commandList;
    static const std::map<std::string, std::string> commandHelp;
};
