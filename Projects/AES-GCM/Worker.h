#pragma once

#include "AES_GCM.h"
#include "header.h"

#include <QByteArray>
#include <QObject>

class Worker : public QObject {
    Q_OBJECT

public:
    Worker(FILE *srcFile, FILE *dstFile, QString dstPath, QByteArray pw, int mode) :
        srcFile(srcFile), dstFile(dstFile), dstPath(dstPath), pw(pw), mode(mode) {
    }

signals:
    void finished(QString msg);
    void progressUpdate(int perc, QString status);

public slots:
    void requestCancel();
    void work();

private:
    FILE *srcFile, *dstFile;
    QByteArray pw;
    QString err = "", dstPath;
    std::atomic<bool> shouldCancel{ false };
    int mode;
};
