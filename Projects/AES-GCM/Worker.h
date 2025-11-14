#pragma once

#include "AES_GCM.h"
#include "header.h"

#include <QByteArray>
#include <QObject>

class Worker : public QObject {
    Q_OBJECT

public:
    Worker(FILE *srcFile, FILE *dstFile, const char *dstPath, QByteArray pw, int mode) :
        srcFile(srcFile), dstFile(dstFile), dstPath(dstPath), pw(pw), mode(mode) {
    }

signals:
    void finished(bool success, QString message);
    void progressUpdate(int prc, QString status);

public slots:
    void requestCancel();
    void work();

private:
    FILE *srcFile, *dstFile;
    QByteArray pw;
    const char *dstPath;
    bool shouldCancel = false;
    int mode;
};
