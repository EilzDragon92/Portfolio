#include "GUI/MainGUI.h"

int ShowGUI(int argc, char *argv[]) {
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

    return app.exec();

    return 0;
}

int main(int argc, char *argv[]) {
    ShowGUI(argc, argv);

    return 0;
}