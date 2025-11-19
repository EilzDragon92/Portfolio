#include "Worker.h"

void Worker::requestCancel() {
    shouldCancel = true;
}

void Worker::work() {
    AES_GCM aes;
    QString msg;
    int res;

    aes.setProgressCb([this](int perc, bool *cancelled) {
        QString status;

        if (mode == 0) {
            status = QString("Encrypting... %1%").arg(perc);
        }
        else {
            status = QString("Decrypting... %1%").arg(perc);
        }

        emit progressUpdate(perc, status);

        *cancelled = shouldCancel;
        });

    if (mode == 0) {
        res = aes.encrypt(srcFile, dstFile, pw);

        if (shouldCancel) {
            msg = "Encryption canceled";
            _unlink(dstPath);
        }
        else if (res) {
            msg = "Encryption failed";
            _unlink(dstPath);
        }
        else {
            msg = "Encryption complete";
        }
    }
    else {
        res = aes.decrypt(srcFile, dstFile, pw);

        if (shouldCancel) {
            msg = "Decryption canceled";
            _unlink(dstPath);
        }
        else if (res) {
            msg = "Decryption failed";
            _unlink(dstPath);
        }
        else {
            msg = "Decryption complete";
        }
    }

    SecureZeroMemory(pw.data(), pw.size());

    emit finished(msg);
}