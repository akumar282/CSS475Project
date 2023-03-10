#pragma once

#include "error.h"

#include <iostream>
#include <map>
#include <list>
#include <iomanip>

class Command {

    std::string command;
    std::list<std::string> args;

public:

// constructors
    Command();
    Command(const std::string&, const std::list<std::string>);
    Command(const Command&);
    
// ostream for debug
    friend std::ostream& operator<<(std::ostream&, const Command&);

// get
    std::string getCommand() const;
    const std::list<std::string>& getArgs() const;
};