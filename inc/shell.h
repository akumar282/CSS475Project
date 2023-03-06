#pragma once

#include <sstream>

#include <../inc/command.h>

class Shell {

bool running;

public:

Shell();

void start();
const Command& fetchCommand();

};
