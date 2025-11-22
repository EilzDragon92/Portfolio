#include "MainGUI.h"
#include "header.h"

#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainGUI *gui = new MainGUI;

    QFont font;
    font.setPointSizeF(font.pointSizeF() * FONT_SCALE);
    QApplication::setFont(font);

    QSize qsize(300, 150);
    gui->resize(qsize);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect rect = screen->availableGeometry();

    int x = (rect.width() - gui->width()) / 2;
    int y = (rect.height() - gui->height()) / 2 - 50;

    gui->move(x, y);

    gui->show();

    while (!gui->hasValidInput() && gui->isVisible()) {
        QApplication::processEvents();
        QThread::msleep(10);
    }

    UserInput input = gui->getUserInput();

    if (!input.valid || !gui) return 1;

    gui->startWork();

    return app.exec();
}