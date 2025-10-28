#include "header.h"

FILE *srcFile, *dstFile;
UserInput userInput;
int pnum;

int OpenFiles(int argc, char *argv[]) {
    userInput = GetUserInput(argc, argv);

    string tmp0 = userInput.src.toStdString();
    string tmp1 = userInput.dst.toStdString();

    const char *srcPath = tmp0.c_str();
    const char *dstPath = tmp1.c_str();

    if (!userInput.valid) {
        printf("ERROR: Input is invalid\n");
        return 1;
    }

    if (fopen_s(&srcFile, srcPath, "rb")) {
        printf("ERROR: Failed to open source file\n");
        return 1;
    }

    if (_access(dstPath, 0) != -1) {
        printf("ERROR: Destination file already exists\n");
        return 1;
    }

    if (fopen_s(&dstFile, dstPath, "wb+")) {
        printf("ERROR: Failed to create destination file\n");
        return 1;
    }

    return 0;
}

int GetProcNum() {
    SYSTEM_INFO systemInfo;

    GetSystemInfo(&systemInfo);

    return systemInfo.dwNumberOfProcessors;
}

int main(int argc, char *argv[]) {
    if (OpenFiles(argc, argv)) {
        if (srcFile) fclose(srcFile);
        return 1;
    }

    pnum = GetProcNum();

    fclose(srcFile);
    fclose(dstFile);

    return 0;
}