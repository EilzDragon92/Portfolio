/**
 * @file	ChangePWGUI.h
 * @brief	Master password change GUI
 * @author	EilzDragon92
 */

#pragma once

#include "Common/header.h"
#include "GUI/PWLineEdit.h"
#include "Utils/Password.h"

/**
 * @class	ChangePWGUI
 * @brief	Dialog for changing the master password
 */
class ChangePWGUI : public QDialog {
	Q_OBJECT

public:
	/**
	 * @brief	Constructor of ChangePWGUI class
	 * @param	parent	Parent widget
	 */
	explicit ChangePWGUI(QWidget *parent = nullptr);

	/**
	 * @brief	Get the change password input from the dialog
	 * @return	Change password input parameters
	 */
	void getInput(Password &curPw, Password &newPw);

	/**
	 * @brief	Reset all input fields
	 */
	void reset();

	/**
	 * @brief	Display error message
	 * @param	msg		Error message string
	 */
	void setErrMsg(const QString &msg);

private slots:
	/**
	 * @brief	Validate input and accept dialog
	 */
	void onOKClicked();

private:
	PWLineEdit *curPWLine, *newPWLine, *confirmPWLine;
	QLabel *curLabel, *newLabel, *confirmLabel, *errMsg;
	QPushButton *okBtn, *cancelBtn;
	QHBoxLayout *btnBox;
	QVBoxLayout *vBox;
};