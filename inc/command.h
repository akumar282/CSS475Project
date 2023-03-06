#pragma once

#include <iostream>
#include <map>
#include <list>

class Command {

    int commandId;
    std::string command;
    std::list<std::string> args;

public:

    Command();
    Command(const std::string&, const std::list<std::string>);
    Command(const Command&);

    static const std::map<std::string, int> commands;

    static void help();

    friend std::ostream& operator<<(std::ostream&, const Command&);

// get
    int getCommandId() const;
    std::string getCommand() const;
    std::list<std::string> getArgs() const;


};