#pragma once

#include <QBoxLayout>
#include <QButtonGroup>
#include <QRadioButton>
#include <QWidget>

class ModeButton : public QWidget {
public:
    explicit ModeButton(QWidget *parent = nullptr);

    int getMode();

private:
    QButtonGroup *btnGroup;
    QHBoxLayout *hBox;
    QRadioButton *radioBtn0;
    QRadioButton *radioBtn1;
};