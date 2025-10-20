#include "header.h"

int main(int argc, char *argv[]) {
    UserInput userInput = show_gui(argc, argv);
    
    printf("%d\n", userInput.valid);
    printf("%d\n", userInput.mode);
    printf("%s\n", userInput.src.toStdString().c_str());
    printf("%s\n", userInput.dst.toStdString().c_str());
    printf("%s\n", userInput.key.toStdString().c_str());

    return 0;
}