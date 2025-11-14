#pragma once

#include "ModeButton.h"
#include "PWLineEdit.h"
#include "header.h"

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

class InputWidget : public QWidget {
    Q_OBJECT

public:
    explicit InputWidget(QWidget *parent = nullptr);

signals:
    void startRequested(const UserInput &input);

private slots:
    void onStartClicked();

private:
    ModeButton *modeBtn;
    PWLineEdit *pwLine;
    QLineEdit *srcLine, *dstLine;
    QPushButton *startBtn;
    QLabel *errMsg;
    QHBoxLayout *hBox;
    QVBoxLayout *vBox;
};