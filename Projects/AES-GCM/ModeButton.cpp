#include "ModeButton.h"

ModeButton::ModeButton(QWidget *parent) : QWidget(parent) {
    hBox = new QHBoxLayout;
    radioBtn0 = new QRadioButton("Encrypt");
    radioBtn1 = new QRadioButton("Decrypt");

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

int ModeButton::getMode() {
    QAbstractButton *btn = btnGroup->checkedButton();
    return btn ? btnGroup->id(btn) : -1;
}