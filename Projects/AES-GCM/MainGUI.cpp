#include <MainGUI.h>

MainGUI::MainGUI(QWidget *parent) : QWidget(parent) {
    iWidget = new InputWidget;
    pWidget = new ProgressWidget;
    sWidget = new QStackedWidget;
    vBox = new QVBoxLayout(this);

    sWidget->addWidget(iWidget);
    sWidget->addWidget(pWidget);

    vBox->addWidget(sWidget);
    vBox->setContentsMargins(0, 0, 0, 0);

    setLayout(vBox);
    setWindowTitle("AES-GCM");

    connect(iWidget, &InputWidget::startRequested, this, &MainGUI::onStartRequested);
    connect(pWidget, &ProgressWidget::closeRequested, this, &QWidget::close);
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

int MainGUI::startProcessing() {
    if (!userInput.valid) return 1;

    if (openFiles()) return 1;

    pWidget->reset();
    sWidget->setCurrentWidget(pWidget);
    resize(300, 150);

    thread = new QThread(this);
    worker = new Worker(srcFile, dstFile, userInput.dst.toStdString().c_str(), userInput.pw.toUtf8(), userInput.mode);
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &Worker::work);
    connect(worker, &Worker::progressUpdate, this, &MainGUI::onProgressUpdated);
    connect(worker, &Worker::finished, this, &MainGUI::onWorkFinished);
    connect(pWidget, &ProgressWidget::cancelRequested, worker, &Worker::requestCancel);
    connect(worker, &Worker::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, this, &MainGUI::onThreadFinished);

    thread->start();

    return 0;
}

void MainGUI::onStartRequested(const UserInput &input) {
    userInput = input;
}

void MainGUI::onProgressUpdated(int percentage, QString status) {
    pWidget->update(percentage, status);
}

void MainGUI::onWorkFinished(bool success, QString message) {
    pWidget->showResult(success, message);
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
        printf("ERROR: Failed to open source file\n");
        return 1;
    }

    if (_access(dstPath, 0) != -1) {
        printf("ERROR: Destination file already exists\n");
        fclose(srcFile);
        return 1;
    }

    if (fopen_s(&dstFile, dstPath, "wb+")) {
        printf("ERROR: Failed to create destination file\n");
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