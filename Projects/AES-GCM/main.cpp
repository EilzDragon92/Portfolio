#include "header.h"

FILE *srcFile, *dstFile;
UserInput userInput;

int GetFileSize(FILE *file) {
    if (_fseeki64(file, 0, SEEK_END)) return -1;

    int res = _ftelli64(file);

    rewind(file);

    return res;
}

int GetProcNum() {
    SYSTEM_INFO systemInfo;

    GetSystemInfo(&systemInfo);
    
    return systemInfo.dwNumberOfProcessors;
}

int OpenFiles(int argc, char *argv[]) {
    userInput = GetUserInput(argc, argv);
    const char *srcPath = userInput.src.toStdString().c_str();
    const char *dstPath = userInput.dst.toStdString().c_str();

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

    fileSize = GetFileSize(srcFile);

    if (fileSize == -1) {
        printf("ERROR: Failed to read file size\n");
        return;
    }
}

int main(int argc, char *argv[]) {
    init(argc, argv);

    fclose(srcFile);
    fclose(dstFile);

    return 0;
}