#include "aes-gcm.h"
#include "header.h"

FILE *srcFile, *dstFile;
UserInput userInput;

int OpenFiles() {
    string tmp0 = userInput.src.toStdString();
    string tmp1 = userInput.dst.toStdString();

    const char *srcPath = tmp0.c_str();
    const char *dstPath = tmp1.c_str();

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

void func() {
    AES_GCM aes;
    string pw = userInput.pw.toStdString();
    int res;

    if (userInput.mode == 0) {
        printf("Encryption in progress...\n");

        res = aes.encrypt(srcFile, dstFile, pw.c_str());

        if (res) printf("Encryption failed\n");
        else printf("Encryption complete\n");
    }
    else {
        printf("Decryption in progress...\n");

        res = aes.decrypt(srcFile, dstFile, pw.c_str());

        if (res) printf("Decryption failed\n");
        else printf("Decryption complete\n");
    }
}

int main(int argc, char *argv[]) {
    userInput = GetUserInput(argc, argv);

    if (!userInput.valid) {
        printf("ERROR: Input is incomplete\n");
        return 1;
    }

    if (OpenFiles()) {
        if (srcFile) fclose(srcFile);
        return 1;
    }

    pnum = GetProcNum();

    func();

    fclose(srcFile);
    fclose(dstFile);

    return 0;
}