#include "aes-gcm.h"
#include "header.h"

using namespace std;

int main(int argc, char *argv[]) {
    /* Get user input from GUI */

    UserInput userInput = GetUserInput(argc, argv);

    if (!userInput.valid) {
        printf("ERROR: Input is incomplete\n");
        return 1;
    }


    /* Open files */

    FILE *srcFile = nullptr, *dstFile = nullptr;
    QByteArray srcBytes = userInput.src.toLocal8Bit();
    QByteArray dstBytes = userInput.dst.toLocal8Bit();
    const char *srcPath = srcBytes.constData();
    const char *dstPath = dstBytes.constData();

    if (fopen_s(&srcFile, srcPath, "rb")) {
        printf("ERROR: Failed to open source file\n");
        return 1;
    }

    if (_access(dstPath, 0) != -1) {
        printf("ERROR: Destination file already exists\n");
        fclose(srcFile);
        return 1;
    }

    if (fopen_s(&dstFile, dstPath, "wb+")) {
        printf("ERROR: Failed to create destination file\n");
        fclose(srcFile);
        return 1;
    }


    /* Get processor number */

    pnum = GetProcNum();


    /* Do encryption or decryption */

    QByteArray pw = userInput.pw.toUtf8();
    AES_GCM aes;

    if (userInput.mode == 0) {
        printf("Encryption in progress...\n");

        if (aes.encrypt(srcFile, dstFile, pw)) {
            printf("Encryption failed\n");
            _unlink(dstPath);
        }
        else {
            printf("Encryption complete\n");
        }
    }
    else {
        printf("Decryption in progress...\n");

        if (aes.decrypt(srcFile, dstFile, pw)) {
            printf("Decryption failed\n");
            _unlink(dstPath);
        }
        else {
            printf("Decryption complete\n");
        }
    }

    SecureZeroMemory(pw.data(), pw.size());


    /* Close files */

    fclose(srcFile);
    fclose(dstFile);

    return 0;
}