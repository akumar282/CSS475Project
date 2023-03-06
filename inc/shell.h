#pragma once

#include "command.h"

#include <iostream>
#include <sstream>

class Shell {

    bool running;

public:

    Shell();

    void start();
    const Command& fetchCommand();

};
