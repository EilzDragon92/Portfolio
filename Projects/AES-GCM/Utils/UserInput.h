#pragma once

#include "Password.h"

#include <QString>

struct UserInput {
    bool valid;
    int mode;
    QString src;
    QString dst;
    Password pw;
};