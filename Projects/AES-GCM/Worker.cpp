#include "Worker.h"

void Worker::requestCancel() {
    shouldCancel = true;
}

void Worker::work() {
    AES_GCM aes;
    QString msg;
    bool res = false;

    aes.setProgressCb([this](int prc, bool *cancelled) {
        QString status;

        if (mode == 0) {
            status = QString("Encrypting... %1%").arg(prc);
        }
        else {
            status = QString("Decrypting... %1%").arg(prc);
        }

        emit progressUpdate(prc, status);

        *cancelled = shouldCancel;
        });

    if (mode == 0) {
        if (aes.encrypt(srcFile, dstFile, pw)) {
            msg = "Encryption failed\n";
            _unlink(dstPath);
        }
        else {
            if (shouldCancel) {
                msg = "Encryption canceled\n";
                _unlink(dstPath);
            }
            else {
                msg = "Encryption complete\n";
                res = true;
            }
        }
    }
    else {
        if (aes.decrypt(srcFile, dstFile, pw)) {
            msg = "Decryption failed\n";
            _unlink(dstPath);
        }
        else {
            if (shouldCancel) {
                msg = "Decryption canceled\n";
                _unlink(dstPath);
            }
            else {
                msg = "Decryption complete\n";
                res = true;
            }
        }
    }

    SecureZeroMemory(pw.data(), pw.size());

    emit finished(res, msg);
}