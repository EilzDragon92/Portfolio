/**
 * @file	PWLineEdit.h
 * @brief	Password input window with show/hide toggle
 * @author	EilzDragon92
 */

#pragma once

#include "Common/header.h"

 /**
  * @class   PWLineEdit
  * @brief   Password input window with show/hide toggle
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
     * @brief   Get the current text in the input field
     * @return  Current text in the input field
     */
    QString getText();

    /**
     * @brief   Clear the input field
     */
    void clear();

private slots:
    /**
     * @brief   Signal when toggle masking button clicked
     */
    void toggleMask();

private:
    QHBoxLayout *hBox;
    QLineEdit *pwLine;
    QPushButton *maskBtn;
};