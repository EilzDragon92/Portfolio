/**
 * @file	MainGUI.h
 * @brief	Main GUI class that controls entire workflow
 * @author	EilzDragon92
 */

#pragma once

#include "Core/Vault.h"
#include "GUI/EntryGUI.h"
#include "GUI/ListGUI.h"
#include "GUI/LoginGUI.h"
#include "GUI/PasswordGUI.h"

/**
 * @class	MainGUI
 * @brief	Main GUI class that orchestrates entire workflow
 */
class MainGUI : public QWidget {
	Q_OBJECT

public:
	/**
	 * @brief	Constructor of MainGUI class
	 * @param	parent	Parent widget
	 */
	explicit MainGUI(QWidget *parent = nullptr);

	/**
	 * @brief	Destructor of MainGUI class
	 */
	~MainGUI();

private slots:
	/**
	 * @brief	Switch to password input screen
	 * @param	mode	0 for new, 1 for open
	 * @param	path	Vault file path
	 */
	void onVaultSelected(int mode, const QString &path);

	/**
	 * @brief	Process vault login request
	 * @param	input	Login input parameters
	 */
	void onLoginRequested(const LoginInput &input);

	/**
	 * @brief	Return to login screen
	 */
	void onBackToLogin();

	/**
	 * @brief	Process entry add request
	 */
	void onAddRequested();

	/**
	 * @brief	Process entry edit request
	 * @param	site	Site of entry to be edited
	 * @param	acc		Account of entry to be edited
	 */
	void onEditRequested(const std::string &site, const std::string &acc);

	/**
	 * @brief	Process entry delete request
	 * @param	site	Site of entry to be deleted
	 * @param	acc		Account of entry to be deleted
	 */
	void onDeleteRequested(const std::string &site, const std::string &acc);

	/**
	 * @brief	Process password generation request
	 */
	void onGenerateRequested();

	/**
	 * @brief	Process vault save request
	 */
	void onSaveRequested();

private:
	EntryGUI *entryGUI;
	ListGUI *listGUI;
	LoginGUI *loginGUI;
	PasswordGUI *pwGUI;
	QStackedWidget *stack;
	QString vaultPath;
	QVBoxLayout *vBox;
	Vault vault;

	std::string editSite;	// Original site for edit mode
	std::string editAcc;	// Original account for edit mode
	bool isEditMode = false;

	/**
	 * @brief	Refresh list GUI
	 */
	void refreshList();
};