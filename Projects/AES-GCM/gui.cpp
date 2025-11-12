#include "aes-gcm.h"
#include "header.h"

#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QFont>
#include <QGuiApplication>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScreen>
#include <QStackedWidget>
#include <QThread>
#include <QWidget>

class ModeButton : public QWidget {
public:
    explicit ModeButton(QWidget *parent = nullptr) : QWidget(parent) {
        auto *hBox = new QHBoxLayout;
        auto *radioBtn0 = new QRadioButton("Encrypt");
        auto *radioBtn1 = new QRadioButton("Decrypt");

        radioBtn0->setChecked(false);
        radioBtn1->setChecked(false);

        btnGroup = new QButtonGroup(this);
        btnGroup->addButton(radioBtn0, 0);
        btnGroup->addButton(radioBtn1, 1);
        btnGroup->setExclusive(true);

        hBox->addWidget(radioBtn0);
        hBox->addWidget(radioBtn1);
        hBox->addStretch();
        hBox->setSpacing(10);
        hBox->setContentsMargins(0, 0, 0, 0);

        setLayout(hBox);
    }

    int getMode() const {
        auto *btn = btnGroup->checkedButton();
        return btn ? btnGroup->id(btn) : -1;
    }

private:
    QButtonGroup *btnGroup;
};

class PWLineEdit : public QWidget {
public:
    explicit PWLineEdit(QWidget *parent = nullptr) : QWidget(parent) {
        auto *hBox = new QHBoxLayout;
        maskBtn = new QPushButton("See");
        pwLine = new QLineEdit;

        pwLine->setPlaceholderText("Password");
        pwLine->setEchoMode(QLineEdit::Password);

        maskBtn->setFixedSize(45, 30);

        hBox->addWidget(pwLine);
        hBox->addWidget(maskBtn);
        hBox->setSpacing(10);
        hBox->setContentsMargins(0, 0, 0, 0);

        connect(maskBtn, &QPushButton::clicked, this, &PWLineEdit::toggleMask);

        setLayout(hBox);
    }
    
    QString getText() {
        return pwLine->text();
    }

private:
    void toggleMask() {
        if (pwLine->echoMode() == QLineEdit::Password) {
            pwLine->setEchoMode(QLineEdit::Normal);
            maskBtn->setText("Hide");
        }
        else {
            pwLine->setEchoMode(QLineEdit::Password);
            maskBtn->setText("See");
        }
    }

    QLineEdit *pwLine;
    QPushButton *maskBtn;
};

class InputWidget : public QWidget {
    Q_OBJECT

public:
    explicit InputWidget(QWidget *parent = nullptr) : QWidget(parent) {
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
        hBox->setContentsMargins(0, 0, 0, 0);

        vBox->addWidget(modeBtn);
        vBox->addWidget(srcLine);
        vBox->addWidget(dstLine);
        vBox->addWidget(pwLine);
        vBox->addLayout(hBox);
        vBox->setSpacing(10);
        vBox->setContentsMargins(10, 10, 10, 10);

        connect(startBtn, &QPushButton::clicked, this, &InputWidget::onStartClicked);

        setLayout(vBox);
    }

signals:
    void startRequested(const UserInput &input);

private slots:
    void onStartClicked() {
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

private:
    ModeButton *modeBtn;
    PWLineEdit *pwLine;
    QLineEdit *srcLine, *dstLine;
    QPushButton *startBtn;
    QLabel *errMsg;
    QHBoxLayout *hBox;
    QVBoxLayout *vBox;
};

class ProgressWidget : public QWidget {
    Q_OBJECT

public:
    explicit ProgressWidget(QWidget *parent = nullptr) : QWidget(parent), cancelled(false) {
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

    void reset() {
        cancelled = false;
        prgBar->setValue(0);
        prgLabel->setText("Initializing...");
        resLabel->clear();
        resLabel->hide();
        cancelBtn->show();
        cancelBtn->setEnabled(true);
        closeBtn->hide();
    }

    void update(int value, const QString &status) {
        prgBar->setValue(value);
        prgLabel->setText(status);
        QApplication::processEvents();
    }

    void showResult(bool success, const QString &message) {
        prgLabel->setText(success ? "Complete" : "Failed");
        prgBar->setValue(success ? 100 : 0);
        cancelBtn->hide();

        resLabel->setText(message);
        resLabel->show();
        closeBtn->show();
    }

    bool isCancelled() const {
        return cancelled;
    }

signals:
    void cancelRequested();
    void closeRequested();

private slots:
    void onCancelClicked() {
        cancelled = true;
        cancelBtn->setEnabled(false);
        prgLabel->setText("Cancelling...");
        emit cancelRequested();
    }

private:
    QLabel *prgLabel, *resLabel;
    QPushButton *cancelBtn, *closeBtn;
    QProgressBar *prgBar;
    QVBoxLayout *vBox;
    bool cancelled;
};

class Worker : public QObject {
    Q_OBJECT

public:
    Worker(FILE *srcFile, FILE *dstFile, const char *dstPath, QByteArray pw, int mode) :
        srcFile(srcFile), dstFile(dstFile), dstPath(dstPath), pw(pw), mode(mode) {
    }

signals:
    void progressUpdate(int prc, QString status);

    void finished(bool success, QString message);

public slots:
    void requestCancel() {
        shouldCancel = true;
    }

    void work() {
        AES_GCM aes;
        QString msg;
        bool res = false;

        aes.setProgressCb([this](int prc, bool *cancelled) {
            QString status;

            if (mode == 0) {
                status = QString("Encrypting... %1%").arg(prc);
            }
            else {
                status = QString("Decrypting... %1%").arg(prc);
            }

            emit progressUpdate(prc, status);

            *cancelled = shouldCancel;
            });

        if (mode == 0) {
            if (aes.encrypt(srcFile, dstFile, pw)) {
                msg = "Encryption failed\n";
                _unlink(dstPath);
            }
            else {
                if (shouldCancel) {
                    msg = "Encryption canceled\n";
                    _unlink(dstPath);
                }
                else {
                    msg = "Encryption complete\n";
                    res = true;
                }
            }
        }
        else {
            if (aes.encrypt(srcFile, dstFile, pw)) {
                msg = "Decryption failed\n";
                _unlink(dstPath);
            }
            else {
                if (shouldCancel) {
                    msg = "Decryption canceled\n";
                    _unlink(dstPath);
                }
                else {
                    msg = "Decryption complete\n";
                    res = true;
                }
            }
        }

        SecureZeroMemory(pw.data(), pw.size());

        emit finished(res, msg);
    }

private:
    FILE *srcFile, *dstFile;
    QByteArray pw;
    const char *dstPath;
    bool shouldCancel;
    int mode;
};

class MainGUI : public QWidget {
    Q_OBJECT

public:
    explicit MainGUI(QWidget *parent = nullptr) : QWidget(parent) {
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

    ~MainGUI() {
        clear();
    }

    UserInput getUserInput() const {
        return userInput;
    }

    bool hasValidInput() const {
        return userInput.valid;
    }

public slots:
    int startProcessing() {
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
        connect(worker, & Worker::finished, thread, &QThread::quit);
        connect(thread, &QThread::finished, this, &MainGUI::onThreadFinished);

        thread->start();

        return 0;
    }

private slots:
    void onStartRequested(const UserInput &input) {
        userInput = input;
    }

    void onProgressUpdated(int percentage, QString status) {
        pWidget->update(percentage, status);
    }

    void onWorkFinished(bool success, QString message) {
        pWidget->showResult(success, message);
    }

    void onThreadFinished() {
        clear();
    }

    void onCloseRequested() {
        close();
    }

private:
    FILE *srcFile, *dstFile;
    InputWidget *iWidget;
    ProgressWidget *pWidget;
    QStackedWidget *sWidget;
    QThread *thread;
    QVBoxLayout *vBox;
    UserInput userInput;
    Worker *worker;

    void clear() {
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

    int openFiles() {
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
};

UserInput GetUserInput(int argc, char *argv[], MainGUI **outMainGUI) {
    QApplication app(argc, argv);
    MainGUI mainWindow;

    QFont font;
    font.setPointSizeF(font.pointSizeF() * 1.2);
    QApplication::setFont(font);

    QSize qsize(300, 150);
    mainWindow.resize(qsize);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect rect = screen->availableGeometry();

    int x = (rect.width() - mainWindow.width()) / 2;
    int y = (rect.height() - mainWindow.height()) / 2 - 50;
    mainWindow.move(x, y);

    mainWindow.show();

    while (!mainWindow.hasValidInput() && mainWindow.isVisible()) {
        QApplication::processEvents();
        QThread::msleep(10);
    }

    UserInput result = mainWindow.getUserInput();

    if (outMainGUI) {
        *outMainGUI = &mainWindow;
    }

    return result;
}

#include "gui.moc"