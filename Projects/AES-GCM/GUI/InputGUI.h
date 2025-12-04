#pragma once

#include "ModeButton.h"
#include "PWLineEdit.h"
#include "UserInput.h"
#include "header.h"

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

class InputGUI : public QWidget {
    Q_OBJECT

public:
    explicit InputGUI(QWidget *parent = nullptr);

    void setErrMsg(const QString &msg);

signals:
    void startRequested(UserInput &input);

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