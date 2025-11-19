#include "PWLineEdit.h"

PWLineEdit::PWLineEdit(QWidget *parent) : QWidget(parent) {
    hBox = new QHBoxLayout;
    maskBtn = new QPushButton("See");
    pwLine = new QLineEdit;

    pwLine->setPlaceholderText("Password");
    pwLine->setEchoMode(QLineEdit::Password);

    maskBtn->setFixedSize(50, pwLine->sizeHint().height() + 5);

    hBox->addWidget(pwLine);
    hBox->addWidget(maskBtn);
    hBox->setSpacing(10);
    hBox->setContentsMargins(0, 0, 0, 0);

    connect(maskBtn, &QPushButton::clicked, this, &PWLineEdit::toggleMask);

    setLayout(hBox);
}

QString PWLineEdit::getText() {
    return pwLine->text();
}

void PWLineEdit::toggleMask() {
    if (pwLine->echoMode() == QLineEdit::Password) {
        pwLine->setEchoMode(QLineEdit::Normal);
        maskBtn->setText("Hide");
    }
    else {
        pwLine->setEchoMode(QLineEdit::Password);
        maskBtn->setText("See");
    }
}