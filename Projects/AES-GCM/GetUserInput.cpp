#include "MainGUI.h"
#include "header.h"

#include <QApplication>
#include <QScreen>

UserInput GetUserInput(int argc, char *argv[], MainGUI **outGUI) {
    MainGUI mainGUI; 
    QApplication app(argc, argv);
    
    QFont font;
    font.setPointSizeF(font.pointSizeF() * 1.2);
    QApplication::setFont(font);

    QSize qsize(300, 150);
    mainGUI.resize(qsize);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect rect = screen->availableGeometry();

    int x = (rect.width() - mainGUI.width()) / 2;
    int y = (rect.height() - mainGUI.height()) / 2 - 50;

    mainGUI.move(x, y);

    mainGUI.show();

    while (!mainGUI.hasValidInput() && mainGUI.isVisible()) {
        QApplication::processEvents();
        QThread::msleep(10);
    }

    UserInput res = mainGUI.getUserInput();

    if (outGUI) *outGUI = &mainGUI;

    return res;
}