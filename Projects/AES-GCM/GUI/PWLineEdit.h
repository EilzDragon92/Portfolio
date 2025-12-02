#pragma once

#include "header.h"

#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

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