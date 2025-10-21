#include "header.h"

#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QFont>
#include <QGuiApplication>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QScreen>
#include <QWidget>

/* Mode selection buttons */
class SelectMode : public QWidget {
public:
    explicit SelectMode(QWidget *parent = nullptr) : QWidget(parent) {
        auto *hBox = new QHBoxLayout;
        auto *radioBtn0 = new QRadioButton("Encrypt");
        auto *radioBtn1 = new QRadioButton("Decrypt");

        /* no button is selected by default */
        radioBtn0->setChecked(false);
        radioBtn1->setChecked(false);

        /* group buttons */
        btnGroup = new QButtonGroup(this);
        btnGroup->addButton(radioBtn0, 0);
        btnGroup->addButton(radioBtn1, 1);
        btnGroup->setExclusive(true);

        /* add components */
        hBox->addWidget(radioBtn0);
        hBox->addWidget(radioBtn1);

        /* configure layout */
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

/* Key input text box */
class KeyLineEdit : public QWidget {
public:
    explicit KeyLineEdit(QWidget *parent = nullptr) : QWidget(parent) {
        auto *hBox = new QHBoxLayout;
        maskBtn = new QPushButton("See");
        lineEdit = new QLineEdit;

        /* apply masking by default */
        lineEdit->setPlaceholderText("Key");
        lineEdit->setEchoMode(QLineEdit::Password);

        /* configure button size */
        maskBtn->setFixedSize(45, 30);

        /* add components */
        hBox->addWidget(lineEdit);
        hBox->addWidget(maskBtn);

        /* configure layout */
        hBox->setSpacing(10);
        hBox->setContentsMargins(0, 0, 0, 0);

        /* add button function */
        connect(maskBtn, &QPushButton::clicked, this, &KeyLineEdit::toggleMasking);

        setLayout(hBox);
    }
    
    QString getText() {
        return lineEdit->text();
    }

private:
    void toggleMasking() {
        if (lineEdit->echoMode() == QLineEdit::Password) {
            lineEdit->setEchoMode(QLineEdit::Normal);
            maskBtn->setText("Hide");
        }
        else {
            lineEdit->setEchoMode(QLineEdit::Password);
            maskBtn->setText("See");
        }
    }

    QLineEdit *lineEdit;
    QPushButton *maskBtn;
};

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr) : QWidget(parent) {
        selectMode = new SelectMode;
        srcLineEdit = new QLineEdit;
        dstLineEdit = new QLineEdit;
        keyLineEdit = new KeyLineEdit;
        startBtn = new QPushButton("Start");
        hBox = new QHBoxLayout;
        vBox = new QVBoxLayout(this);

        /* show roles of each text box */
        srcLineEdit->setPlaceholderText("Source File");
        dstLineEdit->setPlaceholderText("Destination File");

        /* start button sub-layout */
        hBox->addWidget(startBtn);
        hBox->addStretch();
        hBox->setContentsMargins(0, 0, 0, 0);

        /* add components */
        vBox->addWidget(selectMode);
        vBox->addWidget(srcLineEdit);
        vBox->addWidget(dstLineEdit);
        vBox->addWidget(keyLineEdit);
        vBox->addLayout(hBox);

        /* configure main layout */
        vBox->setSpacing(10);
        vBox->setContentsMargins(10, 10, 10, 10);

        /* add button function */
        connect(startBtn, &QPushButton::clicked, this, &MainWindow::start);

        setLayout(vBox);
        setWindowTitle("Cryption");

        /* initialize */
        userInput.valid = 0;
        userInput.mode = -1;
    }

    UserInput getUserInput() const {
        return userInput;
    }

private slots:
    void start() {
        int mode = selectMode->getMode();
        QString src = srcLineEdit->text();
        QString dst = dstLineEdit->text();
        QString key = keyLineEdit->getText();

        if (mode == -1) {
            showError(tr("Mode is not selected."));
            return;
        }

        if (src.isEmpty()) {
            showError(tr("Source file is not input."));
            return;
        }

        if (dst.isEmpty()) {
            showError(tr("Destination file is not input."));
            return;
        }

        if (key.isEmpty()) {
            showError(tr("Key is not input."));
            return;
        }

        userInput.valid = 1;
        userInput.mode = mode;
        userInput.src = src;
        userInput.dst = dst;
        userInput.key = key;

        close();
    }

private:
    void showError(const QString &msg) {
        auto *msgBox = new QMessageBox(this);

        msgBox->setWindowTitle(tr("ERROR"));
        msgBox->setText(msg);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->setAttribute(Qt::WA_DeleteOnClose);
        msgBox->layout()->setContentsMargins(10, 10, 10, 10);
        msgBox->layout()->setSpacing(10);
        msgBox->open();
    }

    SelectMode *selectMode;
    QLineEdit *srcLineEdit, *dstLineEdit;
    KeyLineEdit *keyLineEdit;
    QPushButton *startBtn, *maskingButton;
    QHBoxLayout *keyHBox, *hBox;
    QVBoxLayout *vBox;
    UserInput userInput;
};

UserInput GetUserInput(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow mainWindow;

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

    app.exec();

    return mainWindow.getUserInput();
}

#include "gui.moc"