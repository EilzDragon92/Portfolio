/**
 * @file	PWLineEdit.cpp
 * @brief	Implementation of PWLineEdit class
 * @author	EilzDragon92
 */

#include "GUI/PWLineEdit.h"

PWLineEdit::PWLineEdit(QWidget *parent) : QWidget(parent) {
    /* Create layout and components */

    hBox = new QHBoxLayout;
    maskBtn = new QPushButton("See");
    pwLine = new QLineEdit;


    /* Set placeholder text that indicates password field */

    pwLine->setPlaceholderText("Password");
    pwLine->setEchoMode(QLineEdit::Password);


    /* Configure masking toggle button */

    maskBtn->setFixedSize(45 * FONT_SCALE, pwLine->sizeHint().height() * FONT_SCALE);


    /* Configure layout */

    hBox->addWidget(pwLine);
    hBox->addWidget(maskBtn);
    hBox->setSpacing(10);
    hBox->setContentsMargins(0, 0, 0, 0);


    /* Connect masking toggle function to button */

    connect(maskBtn, &QPushButton::clicked, this, &PWLineEdit::toggleMask);


    setLayout(hBox);
}

QString PWLineEdit::getText() {
    return pwLine->text();
}

void PWLineEdit::clear() {
    pwLine->clear();
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