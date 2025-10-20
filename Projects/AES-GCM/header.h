#pragma once

#include <QString>

/* User input from GUI */
struct UserInput {
    int mode;
    QString src;
    QString dst;
    QString key;
};

UserInput show_gui(int argc, char *argv[]);