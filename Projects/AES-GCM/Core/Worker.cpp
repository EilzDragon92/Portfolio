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
        res = aes.encrypt(srcFile, dstFile, pw.constData(), pw.size());

        if (shouldCancel) {
            msg = "Encryption canceled\n";
            RemoveFile(dstPath.toUtf8().constData());
        }
        else if (res) {
            msg = err + "Encryption failed";
            RemoveFile(dstPath.toUtf8().constData());
        }
        else {
            msg = "Encryption complete\n";
        }
    }
    else {
        res = aes.decrypt(srcFile, dstFile, pw.constData(), pw.size());

        if (shouldCancel) {
            msg = "Decryption canceled\n";
            RemoveFile(dstPath.toUtf8().constData());
        }
        else if (res) {
            msg = "Decryption failed";
            RemoveFile(dstPath.toUtf8().constData());
        }
        else {
            msg = "Decryption complete\n";
        }
    }

    emit finished(msg);
}