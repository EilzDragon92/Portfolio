#include "GUI/MainGUI.h"

MainGUI::MainGUI(QWidget *parent) : QWidget(parent) {
    inputGUI = new InputGUI;
    prgGUI = new ProgressGUI;
    widget = new QStackedWidget;
    vBox = new QVBoxLayout(this);

    widget->addWidget(inputGUI);
    widget->addWidget(prgGUI);

    vBox->addWidget(widget);
    vBox->setContentsMargins(0, 0, 0, 0);

    setLayout(vBox);
    setWindowTitle("AES-GCM");

    connect(inputGUI, &InputGUI::startRequested, this, &MainGUI::onStartRequested);
    connect(prgGUI, &ProgressGUI::closeRequested, this, &QWidget::close);
}

MainGUI::~MainGUI() {
    clear();
}

UserInput MainGUI::getUserInput() {
    return userInput;
}

bool MainGUI::hasValidInput() {
    return userInput.valid;
}

void MainGUI::onStartRequested(const UserInput &input) {
    userInput.valid = input.valid;
    userInput.mode = input.mode;
    userInput.src = input.src;
    userInput.dst = input.dst;
    userInput.pw.setData(input.pw);

    if (openFiles() == 0) {
        widget->setCurrentWidget(prgGUI);

        thread = new QThread(this);
        worker = new Worker(srcFile, dstFile, userInput.dst, userInput.pw, userInput.mode);
        worker->moveToThread(thread);

        connect(thread, &QThread::started, worker, &Worker::work);
        connect(worker, &Worker::progressUpdate, this, &MainGUI::onProgressUpdated);
        connect(worker, &Worker::finished, this, &MainGUI::onWorkFinished);
        connect(prgGUI, &ProgressGUI::cancelRequested, worker, &Worker::requestCancel, Qt::DirectConnection);
        connect(worker, &Worker::finished, thread, &QThread::quit);
        connect(thread, &QThread::finished, this, &MainGUI::onThreadFinished);

        thread->start();

        emit readyToStart();
    }
}

void MainGUI::onProgressUpdated(int perc, QString status) {
    prgGUI->update(perc, status);
}

void MainGUI::onWorkFinished(QString msg, bool shouldDelete) {
    prgGUI->showResult(msg);
    this->shouldDelete = shouldDelete;
}

void MainGUI::onThreadFinished() {
    clear();
}

void MainGUI::onCloseRequested() {
    close();
}

int MainGUI::openFiles() {
    QByteArray srcBytes = userInput.src.toUtf8();
    QByteArray dstBytes = userInput.dst.toUtf8();
    QFileInfo srcInfo(userInput.src);
    QFileInfo dstInfo(userInput.dst);
    const char *srcPath = srcBytes.constData();
    const char *dstPath = dstBytes.constData();

    if (srcInfo.canonicalFilePath() == dstInfo.canonicalFilePath()) {
        inputGUI->setErrMsg("Source and destination cannot be the same");
        return 1;
    }

    if (std::filesystem::exists(dstPath)) {
        inputGUI->setErrMsg("Destination file already exists");
        return 1;
    }

    fopen_s(&srcFile, srcPath, "rb");

    if (srcFile == nullptr) {
        inputGUI->setErrMsg("ERROR: Failed to open source file");
        return 1;
    }

    fopen_s(&dstFile, dstPath, "wb+");

    if (dstFile == nullptr) {
        inputGUI->setErrMsg("Failed to create destination file");
        return 1;
    }

    return 0;
}

void MainGUI::clear() {
    if (thread && thread->isRunning()) {
        if (worker) worker->requestCancel();

        thread->quit();
        thread->wait();
    }

    if (worker) {
        delete worker;
        worker = nullptr;
    }

    if (thread) {
        delete thread;
        thread = nullptr;
    }

    if (srcFile) {
        fclose(srcFile);
        srcFile = nullptr;
    }

    if (dstFile) {
        fclose(dstFile);
        dstFile = nullptr;
    }

    if (shouldDelete) {
        Unlink(userInput.dst.toUtf8().constData());
        shouldDelete = false;
    }
}