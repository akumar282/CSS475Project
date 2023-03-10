#pragma once

#include "command.h"
#include "error.h"
#include "operation.h"
#include "api.h"

#include <iostream>
#include <sstream>

class Shell {

private:

    bool running;
    API api;

    Command fetchCommand();
    error_t executeCommand(const Command&);
    API login();

public:

    Shell();
    void start();

    const API& getAPI();

};
