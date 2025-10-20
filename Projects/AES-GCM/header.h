#pragma once

#include <QString>

/* User input from GUI */
struct UserInput {
    bool valid;
    int mode;
    QString src;
    QString dst;
    QString key;
};

UserInput show_gui(int argc, char *argv[]);