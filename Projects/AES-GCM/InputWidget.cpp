#include "InputWidget.h"

InputWidget::InputWidget(QWidget *parent) : QWidget(parent) {
    modeBtn = new ModeButton;
    srcLine = new QLineEdit;
    dstLine = new QLineEdit;
    pwLine = new PWLineEdit;
    startBtn = new QPushButton("Start");
    errMsg = new QLabel;
    hBox = new QHBoxLayout;
    vBox = new QVBoxLayout;

    srcLine->setPlaceholderText("Source File");
    dstLine->setPlaceholderText("Destination File");

    hBox->addWidget(startBtn);
    hBox->addWidget(errMsg);
    hBox->addStretch();
    hBox->setSpacing(10);
    hBox->setContentsMargins(0, 0, 0, 0);

    vBox->addWidget(modeBtn);
    vBox->addWidget(srcLine);
    vBox->addWidget(dstLine);
    vBox->addWidget(pwLine);
    vBox->addLayout(hBox);
    vBox->addStretch();
    vBox->setSpacing(10);
    vBox->setContentsMargins(10, 10, 10, 10);

    connect(startBtn, &QPushButton::clicked, this, &InputWidget::onStartClicked);

    setLayout(vBox);
}

void InputWidget::onStartClicked() {
    UserInput input;

    input.valid = false;
    input.mode = modeBtn->getMode();
    input.src = srcLine->text();
    input.dst = dstLine->text();
    input.pw = pwLine->getText();

    if (input.mode == -1) {
        errMsg->setText("Mode is not selected");
        return;
    }

    if (input.src.isEmpty()) {
        errMsg->setText("Source file is not input");
        return;
    }

    if (input.dst.isEmpty()) {
        errMsg->setText("Destination file is not input");
        return;
    }

    if (input.pw.isEmpty()) {
        errMsg->setText("Password is not input");
        return;
    }

    input.valid = true;

    emit startRequested(input);
}