#include <QApplication>
#include <QButtonGroup>
#include <QFont>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QScreen>
#include <QVBoxLayout>
#include <QWidget>

class SelectMode : public QWidget {
public:
    explicit SelectMode(QWidget *parent = nullptr) : QWidget(parent) {
        auto *buttonGroup = new QButtonGroup(this);
        auto *hBox = new QHBoxLayout;
        auto *radioButton0 = new QRadioButton("Encrypt");
        auto *radioButton1 = new QRadioButton("Decrypt");

        radioButton0->setChecked(false);
        radioButton1->setChecked(false);

        buttonGroup->addButton(radioButton0, 0);
        buttonGroup->addButton(radioButton1, 1);
        buttonGroup->setExclusive(true);

        hBox->addWidget(radioButton0);
        hBox->addWidget(radioButton1);
        hBox->addStretch();

        hBox->setSpacing(12);
        hBox->setContentsMargins(0, 0, 0, 0);

        setLayout(hBox);
    }
};

class MainWindow : public QWidget {
public:
    explicit MainWindow(QWidget *parent = nullptr) : QWidget(parent) {
        auto *button = new QPushButton("Start");
        auto *lineEdit0 = new QLineEdit;
        auto *lineEdit1 = new QLineEdit;
        auto *lineEdit2 = new QLineEdit;
        auto *selectMode = new SelectMode();
        auto *vBox = new QVBoxLayout(this);

        lineEdit0->setPlaceholderText("Source File");
        lineEdit1->setPlaceholderText("Destination File");
        lineEdit2->setPlaceholderText("Key");

        vBox->addWidget(selectMode);
        vBox->addWidget(lineEdit0);
        vBox->addWidget(lineEdit1);
        vBox->addWidget(lineEdit2);
        vBox->addWidget(button);

        vBox->setSpacing(10);
        vBox->setContentsMargins(10, 10, 10, 10);

        setLayout(vBox);
        setWindowTitle("Cryption");
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow mainWindow;

    QFont font;
    font.setPointSizeF(font.pointSizeF() * 1.25);
    QApplication::setFont(font);

    QSize qsize(300, 100);
    mainWindow.resize(qsize);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect rect = screen->availableGeometry();

    int x = (rect.width() - mainWindow.width()) / 2;
    int y = (rect.height() - mainWindow.height()) / 2 - 50;

    mainWindow.move(x, y);

    mainWindow.show();

    return app.exec();
}