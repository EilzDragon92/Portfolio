#include "Worker.h"

void Worker::requestCancel() {
    shouldCancel = true;
}

void Worker::work() {
    AES_GCM aes;
    QString msg;
    int res;

    aes.setErrorCb([this](const char *msg) {
        err = QString(msg);
        });

    aes.setProgressCb([this](int perc, bool *cancelled) {
        QString status;

        if (mode == 0) {
            status = QString("Encrypting... %1%\n").arg(perc);
        }
        else {
            status = QString("Decrypting... %1%\n").arg(perc);
        }

        emit progressUpdate(perc, status);

        *cancelled = shouldCancel;
        });

    if (mode == 0) {
        res = aes.encrypt(srcFile, dstFile, pw);

        if (shouldCancel) {
            msg = "Encryption canceled\n";
            _unlink(dstPath);
        }
        else if (res) {
            msg = err + "Encryption failed";
            _unlink(dstPath);
        }
        else {
            msg = "Encryption complete\n";
        }
    }
    else {
        res = aes.decrypt(srcFile, dstFile, pw);

        if (shouldCancel) {
            msg = "Decryption canceled\n";
            _unlink(dstPath);
        }
        else if (res) {
            msg = "Decryption failed";
            _unlink(dstPath);
        }
        else {
            msg = "Decryption complete\n";
        }
    }

    SecureZeroMemory(pw.data(), pw.size());

    emit finished(msg);
}