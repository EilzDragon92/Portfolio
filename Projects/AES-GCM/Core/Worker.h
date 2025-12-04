#pragma once

#include "AES_GCM.h"
#include "Password.h"

#include <QByteArray>
#include <QObject>

class Worker : public QObject {
    Q_OBJECT

public:
    Worker(FILE *srcFile, FILE *dstFile, QString &dstPath, Password &pw, int mode) {
        this->srcFile = srcFile;
        this->dstFile = dstFile;
        this->dstPath = dstPath;
        this->pw.setData(pw);
        this->mode = mode;
    }

    ~Worker() {
        if (srcFile) {
            fclose(srcFile);
            srcFile = nullptr;
        }
        if (dstFile) {
            fclose(dstFile);
            dstFile = nullptr;
        }
    }

signals:
    void finished(QString msg);
    void progressUpdate(int perc, QString status);

public slots:
    void requestCancel();
    void work();

private:
    FILE *srcFile, *dstFile;
    QString err = "", dstPath;
    Password pw;
    std::atomic<bool> shouldCancel{ false };
    int mode;
};
