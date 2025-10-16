#include <QApplication>
#include <QButtonGroup>
#include <QFont>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QScreen>
#include <QVBoxLayout>
#include <QWidget>

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QBoxLayout>

class SelectMode : public QWidget {
public:
    explicit SelectMode(QWidget *parent = nullptr) : QWidget(parent) {
        auto *hBox = new QHBoxLayout;
        auto *radioButton0 = new QRadioButton("Encrypt");
        auto *radioButton1 = new QRadioButton("Decrypt");

        radioButton0->setChecked(false);
        radioButton1->setChecked(false);
        
        buttonGroup = new QButtonGroup(this);
        buttonGroup->addButton(radioButton0, 0);
        buttonGroup->addButton(radioButton1, 1);
        buttonGroup->setExclusive(true);

        hBox->addWidget(radioButton0);
        hBox->addWidget(radioButton1);
        hBox->addStretch();
        hBox->setSpacing(10);
        hBox->setContentsMargins(0, 0, 0, 0);

        setLayout(hBox);
    }

    int getMode() const {
        auto *button = buttonGroup->checkedButton();
        return button ? buttonGroup->id(button) : -1;
    }

private:
    QButtonGroup *buttonGroup;
};

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr) : QWidget(parent) {
        selectMode = new SelectMode;
        srcLineEdit = new QLineEdit;
        dstLineEdit = new QLineEdit;
        keyLineEdit = new QLineEdit;
        startButton = new QPushButton("Start");
        hBox = new QHBoxLayout;
        vBox = new QVBoxLayout(this);

        srcLineEdit->setPlaceholderText("Source File");
        dstLineEdit->setPlaceholderText("Destination File");
        keyLineEdit->setPlaceholderText("Key");

        hBox->addWidget(startButton);
        hBox->addStretch();
        hBox->setContentsMargins(0, 0, 0, 0);

        vBox->addWidget(selectMode);
        vBox->addWidget(srcLineEdit);
        vBox->addWidget(dstLineEdit);
        vBox->addWidget(keyLineEdit);
        vBox->addLayout(hBox);
        vBox->setSpacing(10);
        vBox->setContentsMargins(10, 10, 10, 10);

        connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartButtonClicked);

        setLayout(vBox);
        setWindowTitle("Cryption");
    }

private slots:
    int onStartButtonClicked() {
        int mode = selectMode->getMode();
        auto *errMsg = new QMessageBox(this);
        QString src = srcLineEdit->text();
        QString dst = dstLineEdit->text();
        QString key = keyLineEdit->text();
        
        errMsg->setWindowTitle(tr("ERROR"));
        errMsg->setStandardButtons(QMessageBox::Ok);
        errMsg->setAttribute(Qt::WA_DeleteOnClose);

        errMsg->layout()->setContentsMargins(10, 10, 10, 10);
        errMsg->layout()->setSpacing(10);

        if (mode == -1) {
            errMsg->setText(tr("Mode is not selected."));
            errMsg->open();
            return 1;
        }

        if (src.isEmpty()) {
            errMsg->setText(tr("Source file name or path is not input."));
            errMsg->open();
            return 1;
        }

        if (dst.isEmpty()) {
            errMsg->setText(tr("Destination file name or path is not input."));
            errMsg->open();
            return 1;
        }

        if (key.isEmpty()) {
            errMsg->setText(tr("Key is not input."));
            errMsg->open();
            return 1;
        }

        return 0;
    }

private:
    SelectMode *selectMode; 
    QLineEdit *srcLineEdit, *dstLineEdit, *keyLineEdit;
    QPushButton *startButton;
    QHBoxLayout *hBox;
    QVBoxLayout *vBox;
};

int show_gui(int argc, char *argv[]) {
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

    return app.exec();
}

#include "gui.moc"