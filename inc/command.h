#pragma once

#include <map>

class Command {

std::string command;
std::string args[];

public:

static const std::map<std::string, int> commands;

static void help();

};