#pragma once

#include "header.h"

#include <QApplication>
#include <QBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

class ProgressWidget : public QWidget {
    Q_OBJECT

public:
    explicit ProgressWidget(QWidget *parent = nullptr);

    bool isCancelled(); 
    
    void reset();

    void update(int value, const QString &status);

    void showResult(bool success, const QString &message);

signals:
    void cancelRequested();
    void closeRequested();

private slots:
    void onCancelClicked();

private:
    QLabel *prgLabel, *resLabel;
    QPushButton *cancelBtn, *closeBtn;
    QProgressBar *prgBar;
    QVBoxLayout *vBox;
    bool cancelled;
};