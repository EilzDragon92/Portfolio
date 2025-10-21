#include "header.h"

FILE *srcFile, *dstFile;

int OpenFiles(int argc, char *argv[]) {
    UserInput userInput = GetUserInput(argc, argv);
    const char *srcPath = userInput.src.toStdString().c_str();
    const char *dstPath = userInput.dst.toStdString().c_str();

    if (!userInput.valid) {
        printf("ERROR: Input is incomplete\n");
        return 1;
    }

    if (fopen_s(&srcFile, srcPath, "rb")) {
        printf("ERROR: Cannot open source file\n");
        return 1;
    }

    if (_access(dstPath, 0) == -1) {
        printf("ERROR: Destination file already exists\n");
        return 1;
    }

    if (fopen_s(&dstFile, dstPath, "wb+")) {
        printf("ERROR: Cannot create destination file\n");
        return 1;
    }

    return 0;
}

void init(int argc, char *argv[]) {
    if (OpenFiles(argc, argv)) return;
}

void main(int argc, char *argv[]) {
    init(argc, argv);

    fclose(srcFile);
    fclose(dstFile);
}