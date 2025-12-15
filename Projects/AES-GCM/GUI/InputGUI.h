/**
 * @file	InputGUI.h
 * @brief	Input widget for user-provided parameters
 * @author	EilzDragon92
 */

#pragma once

#include "Common/header.h"
#include "GUI/ModeButton.h"
#include "GUI/PWLineEdit.h"
#include "Utils/Password.h"

/**
  * @struct     UserInput
  * @brief      Container for user-provided parameters
  */
struct UserInput {
    bool valid;
    int mode;
    QString src;
    QString dst;
    Password pw;
};

/**
 * @class   InputGUI
 * @brief   Input widget for user-provided parameters
 */
class InputGUI : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief   Constructor of InputGUI class
     * @param   parent  Parent widget
     */
    explicit InputGUI(QWidget *parent = nullptr);

    /**
     * @brief   Display error message
     * @param   msg     error message string
     */
    void setErrMsg(const QString &msg);

signals:
    /**
     * @brief   Signal when start button clicked
     * @param   input   User-provided parameters
     */
    void startRequested(const UserInput &input);

private slots:
    /**
     * @brief   Check the user-provided parameters are valid and start encryption/decryption
     */
    void onStartClicked();

private:
    ModeButton *modeBtn;
    PWLineEdit *pwLine;
    QLineEdit *srcLine;     // Source file input field
    QLineEdit *dstLine;     // Destination file input field
    QPushButton *startBtn;
    QLabel *errMsg;         // Label to display error messages
    QHBoxLayout *hBox;      // Layout for start button and error message
    QVBoxLayout *vBox;
};