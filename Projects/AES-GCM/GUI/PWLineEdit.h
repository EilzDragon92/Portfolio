/**
 * @file	PWLineEdit.h
 * @brief	Password input widget with show/hide toggle
 * @author	EilzDragon92
 */

#pragma once

#include "Common/header.h"

 /**
  * @class   PWLineEdit
  * @brief   Password input widget with show/hide toggle
  */
class PWLineEdit : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief   Constructor of PWLineEdit class
     * @param   parent  Parent widget
     */
    explicit PWLineEdit(QWidget *parent = nullptr);

    /**
     * @brief   Get the current text
     * @return  Current text
     */
    QString getText();

    /**
     * @brief   Clear the input field
     */
    void clear();

private slots:
    /**
     * @brief   Toggle password masking
     */
    void toggleMask();

private:
    QHBoxLayout *hBox;
    QLineEdit *pwLine;
    QPushButton *maskBtn;
};