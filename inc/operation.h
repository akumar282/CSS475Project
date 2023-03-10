#pragma once

#include "command.h"
#include "error.h"
#include "api.h"

#include <pqxx/pqxx>
#include <regex>
#include <iomanip>
#include <random>
#include <string>

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
    static constexpr operation_t c_list = 7;
    static constexpr operation_t c_delay = 8;
    static constexpr operation_t c_meals = 9;
    static constexpr operation_t c_mealTypes = 10;
    static constexpr operation_t c_changeStatus = 11;
    static constexpr operation_t c_changeDestination = 12;
    static constexpr operation_t c_removeCargo = 13;
    static constexpr operation_t c_addCargo = 14;
    static constexpr operation_t c_changeOrigin = 15;
    static constexpr operation_t c_checkCargo = 16;

    // operation functions
    static error_t shell_exit();
    static error_t help();
    static error_t status(const API&, const std::list<std::string>&);
    static error_t create(const API&, const std::list<std::string>&);
    static error_t depart(const API&, const std::list<std::string>&);
    static error_t arrive(const API&, const std::list<std::string>&);
    static error_t passengers(const API&, const std::list<std::string>&);
    static error_t addCargo(const API&, const std::list<std::string>&);
    static error_t removeCargo(const API&, const std::list<std::string>&);
    static error_t checkCargo(const API &, const std::list<std::string> &);
    static error_t list(const API&);
    static error_t delay(const API&, const std::list<std::string>&);
    static error_t mealTypes(const API&, const std::list<std::string>&);
    static error_t meals(const API&, const std::list<std::string>&);
    static error_t changeStatus(const API&, const std::list<std::string>&);
    static error_t changeDestination(const API&, const std::list<std::string>&);
    static error_t changeOrigin(const API &, const std::list<std::string> &);

    // mappings
    static const std::map<std::string, operation_t> commandList;
    static const std::map<std::string, std::string> commandHelp;
};
