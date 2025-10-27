#include "header.h"

FILE *srcFile, *dstFile;
UserInput userInput;

int GetProcNum() {
    SYSTEM_INFO systemInfo;

    GetSystemInfo(&systemInfo);
    
    return systemInfo.dwNumberOfProcessors;
}

int OpenFiles(int argc, char *argv[]) {
    userInput = GetUserInput(argc, argv);

    string tmp0 = userInput.src.toStdString();
    string tmp1 = userInput.dst.toStdString();

    const char *srcPath = tmp0.c_str();
    const char *dstPath = tmp1.c_str();

    if (!userInput.valid) return 1;

    if (fopen_s(&srcFile, srcPath, "rb")) return 2;

    if (_access(dstPath, 0) != -1) return 3;

    if (fopen_s(&dstFile, dstPath, "wb+")) return 4;

    return 0;
}

void init(int argc, char *argv[]) {
    int res = OpenFiles(argc, argv);

    if (res == 1) {
        printf("ERROR: Input is incomplete\n");
        return;
    }

    if (res == 2) {
        printf("ERROR: Failed to open source file\n");
        return;
    }

    if (res == 3) {
        printf("ERROR: Destination file already exists\n");
        return;
    }

    if (res == 4) {
        printf("ERROR: Failed to create destination file\n");
        return;
    }
}

int main(int argc, char *argv[]) {
    init(argc, argv);

    fclose(srcFile);
    fclose(dstFile);

    return 0;
}