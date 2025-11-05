#include "aes-gcm.h"
#include "header.h"

using namespace std;

int OpenFiles(FILE **srcFile, FILE **dstFile, const char *srcPath, const char *dstPath) {
    if (fopen_s(srcFile, srcPath, "rb")) {
        printf("ERROR: Failed to open source file\n");
        return 1;
    }

    if (_access(dstPath, 0) != -1) {
        printf("ERROR: Destination file already exists\n");
        fclose(*srcFile);
        return 1;
    }

    if (fopen_s(dstFile, dstPath, "wb+")) {
        printf("ERROR: Failed to create destination file\n");
        fclose(*srcFile);
        return 1;
    }

    return 0;
}

void Cryption(FILE *src, FILE *dst, const char *pw, int mode) {
    AES_GCM aes;
    int res;

    if (mode == 0) {
        printf("Encryption in progress...\n");

        res = aes.encrypt(src, dst, pw);

        if (res) printf("Encryption failed\n");
        else printf("Encryption complete\n");
    }
    else {
        printf("Decryption in progress...\n");

        res = aes.decrypt(src, dst, pw);

        if (res) printf("Decryption failed\n");
        else printf("Decryption complete\n");
    }
}

int main(int argc, char *argv[]) {
    /* Get user input from GUI */

    UserInput userInput = GetUserInput(argc, argv);

    if (!userInput.valid) {
        printf("ERROR: Input is incomplete\n");
        return 1;
    }


    /* Open files */

    FILE *srcFile = nullptr, *dstFile = nullptr;

    string tmp0 = userInput.src.toStdString();
    string tmp1 = userInput.dst.toStdString();

    const char *srcPath = tmp0.c_str();
    const char *dstPath = tmp1.c_str();

    if (OpenFiles(&srcFile, &dstFile, srcPath, dstPath)) return 1;


    /* Get processor number */

    pnum = GetProcNum();


    /* Do encryption or decryption */

    string tmp2 = userInput.pw.toStdString();
    const char *pw = tmp2.c_str();

    Cryption(srcFile, dstFile, pw, userInput.mode);


    /* Close files */

    fclose(srcFile);
    fclose(dstFile);

    return 0;
}