#include <MainGUI.h>

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

int MainGUI::startWork() {
    if (!userInput.valid) return 1;

    if (openFiles()) {
        widget->setCurrentWidget(inputGUI);
        return 1;
    }

    widget->setCurrentWidget(prgGUI);

    thread = new QThread(this);
    worker = new Worker(srcFile, dstFile, userInput.dst.toStdString().c_str(), userInput.pw.toUtf8(), userInput.mode);
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &Worker::work);
    connect(worker, &Worker::progressUpdate, this, &MainGUI::onProgressUpdated);
    connect(worker, &Worker::finished, this, &MainGUI::onWorkFinished);
    connect(prgGUI, &ProgressGUI::cancelRequested, worker, &Worker::requestCancel, Qt::DirectConnection);
    connect(worker, &Worker::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, this, &MainGUI::onThreadFinished);

    thread->start();

    return 0;
}

void MainGUI::onStartRequested(const UserInput &input) {
    userInput = input;
}

void MainGUI::onProgressUpdated(int perc, QString status) {
    prgGUI->update(perc, status);
}

void MainGUI::onWorkFinished(QString msg) {
    prgGUI->showResult(msg);
}

void MainGUI::onThreadFinished() {
    clear();
}

void MainGUI::onCloseRequested() {
    close();
}

int MainGUI::openFiles() {
    QByteArray srcBytes = userInput.src.toLocal8Bit();
    QByteArray dstBytes = userInput.dst.toLocal8Bit();
    const char *srcPath = srcBytes.constData();
    const char *dstPath = dstBytes.constData();

    if (fopen_s(&srcFile, srcPath, "rb")) {
        inputGUI->setErrMsg("ERROR: Failed to open source file");
        return 1;
    }

    if (_access(dstPath, 0) != -1) {
        inputGUI->setErrMsg("Destination file already exists");
        fclose(srcFile);
        return 1;
    }

    if (fopen_s(&dstFile, dstPath, "wb+")) {
        inputGUI->setErrMsg("Failed to create destination file");
        fclose(srcFile);
        return 1;
    }

    return 0;
}

void MainGUI::clear() {
    if (thread) {
        if (thread->isRunning()) {
            thread->quit();
            thread->wait();
        }

        worker = nullptr;
        thread = nullptr;
    }

    if (srcFile) fclose(srcFile);
    if (dstFile) fclose(dstFile);
}