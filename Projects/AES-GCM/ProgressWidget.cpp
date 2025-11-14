#include <ProgressWidget.h>

ProgressWidget::ProgressWidget(QWidget *parent) : QWidget(parent), cancelled(false) {
    prgLabel = new QLabel("Initializing...");
    resLabel = new QLabel;
    prgBar = new QProgressBar;
    cancelBtn = new QPushButton("Cancel");
    closeBtn = new QPushButton("Close");
    vBox = new QVBoxLayout;

    prgBar->setRange(0, 100);
    prgBar->setValue(0);
    resLabel->setWordWrap(true);
    resLabel->hide();
    closeBtn->hide();

    vBox->addWidget(prgLabel);
    vBox->addWidget(prgBar);
    vBox->addWidget(cancelBtn);
    vBox->addWidget(resLabel);
    vBox->addWidget(closeBtn);
    vBox->addStretch();
    vBox->setSpacing(10);
    vBox->setContentsMargins(10, 10, 10, 10);

    connect(cancelBtn, &QPushButton::clicked, this, &ProgressWidget::onCancelClicked);
    connect(closeBtn, &QPushButton::clicked, this, &ProgressWidget::closeRequested);

    setLayout(vBox);
}

bool ProgressWidget::isCancelled() {
    return cancelled;
}

void ProgressWidget::reset() {
    cancelled = false;
    prgBar->setValue(0);
    prgLabel->setText("Initializing...");
    resLabel->clear();
    resLabel->hide();
    cancelBtn->show();
    cancelBtn->setEnabled(true);
    closeBtn->hide();
}

void ProgressWidget::update(int value, const QString &status) {
    prgBar->setValue(value);
    prgLabel->setText(status);
    QApplication::processEvents();
}

void ProgressWidget::showResult(bool success, const QString &message) {
    prgLabel->setText(success ? "Complete" : "Failed");
    prgBar->setValue(success ? 100 : 0);
    cancelBtn->hide();

    resLabel->setText(message);
    resLabel->show();
    closeBtn->show();
}

void ProgressWidget::onCancelClicked() {
    cancelled = true;
    cancelBtn->setEnabled(false);
    prgLabel->setText("Cancelling...");
    emit cancelRequested();
}