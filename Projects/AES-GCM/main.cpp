#include "MainGUI.h"
#include "header.h"

#include <QApplication>
#include <QScreen>

UserInput OpenGUI(int argc, char *argv[], MainGUI **dst) {
    MainGUI gui;

    QFont font;
    font.setPointSizeF(font.pointSizeF() * 1.2);
    QApplication::setFont(font);

    QSize qsize(300, 150);
    gui.resize(qsize);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect rect = screen->availableGeometry();

    int x = (rect.width() - gui.width()) / 2;
    int y = (rect.height() - gui.height()) / 2 - 50;

    gui.move(x, y);

    gui.show();

    while (!gui.hasValidInput() && gui.isVisible()) {
        QApplication::processEvents();
        QThread::msleep(10);
    }

    UserInput res = gui.getUserInput();

    if (dst) *dst = &gui;

    return res;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainGUI *gui = nullptr;
    UserInput input = OpenGUI(argc, argv, &gui);

    if (!input.valid || !gui) {
        printf("ERROR: Input is invalid or or GUI is closed\n");
        return 1;
    }

    pnum = GetProcNum();

    gui->startProcessing();

    return app.exec();
}