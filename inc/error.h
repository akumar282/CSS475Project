#pragma once

// defines error ids
typedef int error_t;

class Error {
public:
    // error ids
    static const int EXIT = 0;
    static const int SUCCESS = 1;
    static const int BADARGS = 2;
    static const int BADCMD = 3;
    static const int DBERROR = 4;
};
