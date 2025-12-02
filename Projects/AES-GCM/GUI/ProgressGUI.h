#pragma once

#include "header.h"

#include <QApplication>
#include <QBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

class ProgressGUI : public QWidget {
    Q_OBJECT

public:
    explicit ProgressGUI(QWidget *parent = nullptr);

    bool isCancelled();

    void update(int val, const QString &status);

    void showResult(const QString &msg);

signals:
    void cancelRequested();
    void closeRequested();

private slots:
    void onCancelClicked();

private:
    QLabel *prgLabel;
    QProgressBar *prgBar;
    QPushButton *cancelBtn, *closeBtn;
    QHBoxLayout *hBox;
    QVBoxLayout *vBox;
    bool cancelled;
};