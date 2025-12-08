#pragma once

#include "Core/AES_GCM.h"
#include "Utils/File.h"
#include "Utils/Password.h"

class Worker : public QObject {
    Q_OBJECT

public:
    Worker(FILE *srcFile, FILE *dstFile, QString dstPath, Password pw, int mode) {
        this->srcFile = srcFile;
        this->dstFile = dstFile;
        this->dstPath = dstPath;
        this->pw.setData(pw);
        this->mode = mode;
    }

signals:
    void finished(QString msg);
    void progressUpdate(int perc, QString status);

public slots:
    void requestCancel();
    void work();

private:
    FILE *srcFile = nullptr, *dstFile = nullptr;
    QString err = "", dstPath;
    Password pw;
    std::atomic<bool> shouldCancel{ false };
    int mode;
};
