#pragma once

#include "Common/header.h"

class PWLineEdit : public QWidget {
    Q_OBJECT

public:
    explicit PWLineEdit(QWidget *parent = nullptr);

    QString getText();

    void clear();

private slots:
    void toggleMask();

private:
    QHBoxLayout *hBox;
    QLineEdit *pwLine;
    QPushButton *maskBtn;
};