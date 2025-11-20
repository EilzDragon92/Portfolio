#include <ProgressGUI.h>

ProgressGUI::ProgressGUI(QWidget *parent) : QWidget(parent), cancelled(false) {
    prgLabel = new QLabel("Initializing...");
    prgBar = new QProgressBar;
    cancelBtn = new QPushButton("Cancel");
    closeBtn = new QPushButton("Close");
    hBox = new QHBoxLayout;
    vBox = new QVBoxLayout;

    prgBar->setRange(0, 100);
    prgBar->setValue(0);

    closeBtn->hide();

    hBox->addWidget(cancelBtn);
    hBox->addWidget(closeBtn);
    hBox->addStretch();
    hBox->setSpacing(10);
    hBox->setContentsMargins(0, 0, 0, 0);

    vBox->addWidget(prgLabel);
    vBox->addWidget(prgBar);
    vBox->addStretch();
    vBox->addLayout(hBox);
    vBox->setSpacing(10);
    vBox->setContentsMargins(10, 10, 10, 10);

    setLayout(vBox);

    connect(cancelBtn, &QPushButton::clicked, this, &ProgressGUI::onCancelClicked);
    connect(closeBtn, &QPushButton::clicked, this, &ProgressGUI::closeRequested);
}

bool ProgressGUI::isCancelled() {
    return cancelled;
}

void ProgressGUI::update(int val, const QString &status) {
    prgBar->setValue(val);
    prgLabel->setText(status);
}

void ProgressGUI::showResult(const QString &msg) {
    prgLabel->setText(msg);
    cancelBtn->hide();
    closeBtn->show();
}

void ProgressGUI::onCancelClicked() {
    cancelled = true;
    emit cancelRequested();
}