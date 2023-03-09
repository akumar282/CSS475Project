#pragma once

#include "command.h"
#include "error.h"
#include "operation.h"

#include <iostream>
#include <sstream>

class Shell {

    bool running;

public:

    Shell();

    void start();
    Command fetchCommand();
    error_t executeCommand(const Command&);

};
