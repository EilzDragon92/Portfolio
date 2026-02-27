/**
 * @file	main.cpp
 * @brief	Entry point of the application
 * @author	EilzDragon92
 */

#include "Common/header.h"
#include "GUI/LoginGUI.h"
#include "GUI/PasswordGUI.h"

int ShowGUI(int argc, char *argv[]) {
    QApplication app(argc, argv);
    //LoginGUI *gui = new LoginGUI;
    PasswordGUI *gui = new PasswordGUI;


    /* Configure font size */

    QFont font;
    font.setPointSizeF(font.pointSizeF() * FONT_SCALE);
    QApplication::setFont(font);


    /* Configure window size */

    QSize qsize(300, 150);
    gui->resize(qsize);


    /* Configure window position */

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect rect = screen->availableGeometry();

    int x = (rect.width() - gui->width()) / 2;
    int y = (rect.height() - gui->height()) / 2 - 50;

    gui->move(x, y);

    gui->show();


    return app.exec();
}

int main(int argc, char *argv[]) {
    return ShowGUI(argc, argv);
}