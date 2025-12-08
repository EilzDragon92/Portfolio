#pragma once

#include "Common/header.h"
#include "GUI/ModeButton.h"
#include "GUI/PWLineEdit.h"
#include "Utils/Password.h"

struct UserInput {
    bool valid;
    int mode;
    QString src;
    QString dst;
    Password pw;
};

class InputGUI : public QWidget {
    Q_OBJECT

public:
    explicit InputGUI(QWidget *parent = nullptr);

    void setErrMsg(const QString &msg);

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