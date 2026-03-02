/**
 * @file	EntryGUI.h
 * @brief	Password entry add & edit window
 * @author	EilzDragon92
 */

#pragma once

#include "Common/header.h"
#include "Core/Entry.h"
#include "GUI/PWLineEdit.h"
#include "Utils/Password.h"

/**
 * @class	EntryGUI
 * @brief	Dialog for adding and editing password entries
 */
class EntryGUI : public QDialog {
	Q_OBJECT

public:
	/**
	 * @brief	Constructor of EntryGUI class
	 * @param	parent	Parent widget
	 */
	explicit EntryGUI(QWidget *parent = nullptr);

	/**
	 * @brief	Set dialog to add mode
	 */
	void setAddMode();

	/**
	 * @brief	Set dialog to edit mode with existing data
	 * @param	site	Current site name
	 * @param	acc		Current account
	 * @param	pw		Current password
	 */
	void setEditMode(const std::string &site, const std::string &acc, const Password &pw);

	/**
	 * @brief	Get the entry input from the dialog
	 * @return	Entry input parameters
	 */
	Entry getInput();

	/**
	 * @brief	Set the generated password to the password field
	 * @param	pw	Generated password
	 */
	void setPassword(const Password &pw);

	/**
	 * @brief	Get the special character selection list
	 * @return	Special character selection list
	 */
	std::vector<bool> getSpecialsList();

	/**
	 * @brief	Get the password length
	 * @return	Password length
	 */
	int getPasswordSize();

signals:
	/**
	 * @brief	Signal when password generation is requested
	 */
	void generateRequested();

private slots:
	/**
	 * @brief	Validate input and accept dialog
	 */
	void onOKClicked();

private:
	PWLineEdit *pwLine; 
	QCheckBox *spcChecks[32];
	QGridLayout *spcGrid;
	QLabel *errMsg;
	QLabel *lenLabel;
	QLineEdit *siteLine;
	QLineEdit *accLine;
	QPushButton *genBtn;
	QPushButton *okBtn;
	QPushButton *cancelBtn;
	QSlider *lenSlider;
	QHBoxLayout *btnBox;
	QHBoxLayout *lenBox;
	QVBoxLayout *vBox;

	static constexpr char spcs[] = "`~!@#$%^&*()-_=+[{]}\\|;:\'\",<.>/?";
};