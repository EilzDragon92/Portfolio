#pragma once

#include <QString>
#include <io.h>

using namespace std;

/* User input from GUI */
struct UserInput {
    bool valid;
    int mode;
    QString src;
    QString dst;
    QString key;
};

UserInput GetUserInput(int argc, char *argv[]);