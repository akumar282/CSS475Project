#pragma once

#include "command.h"
#include "error.h"
#include "api.h"

#include <pqxx/pqxx>
#include <regex>

// defines operation ids for jump table
typedef int operation_t;

class Operation {
public:

    // operation ids
    static constexpr operation_t c_exit = 0;
    static constexpr operation_t c_help = 1;
    static constexpr operation_t c_status = 2;
    static constexpr operation_t c_create = 3;
    static constexpr operation_t c_depart = 4;
    static constexpr operation_t c_arrive = 5;
    static constexpr operation_t c_passengers = 6;

    // operation functions
    static error_t shell_exit();
    static error_t help();
    static error_t status(const API&, const std::list<std::string>&);
    static error_t create(const API&, const std::list<std::string>&);
    static error_t depart(const API&, const std::list<std::string>&);
    static error_t arrive(const API&, const std::list<std::string>&);
    static error_t passengers(const API&, const std::list<std::string>&);
    static error_t addCargo(const API&, const std::list<std::string>&);
    
    // mappings
    static const std::map<std::string, operation_t> commandList;
    static const std::map<std::string, std::string> commandHelp;
};
