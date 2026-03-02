/**
 * @file	MainGUI.cpp
 * @brief	Implementation of MainGUI class
 * @author	EilzDragon92
 */

#include "GUI/MainGUI.h"

MainGUI::MainGUI(QWidget *parent) : QWidget(parent) {
	/* Create layouts and components */

	loginGUI = new LoginGUI(this);
	pwGUI = new PasswordGUI(this);
	listGUI = new ListGUI(this);
	entryGUI = new EntryGUI(this);
	stack = new QStackedWidget(this);
	vBox = new QVBoxLayout(this);


	/* Add GUIs to stacked widget for switching */

	stack->addWidget(loginGUI);
	stack->addWidget(pwGUI);
	stack->addWidget(listGUI);


	/* Configure layout */

	vBox->addWidget(stack);
	vBox->setContentsMargins(0, 0, 0, 0);

	setLayout(vBox);
	setWindowTitle("PasswordManager");


	/* Connect login signals */

	connect(loginGUI, &LoginGUI::vaultSelected, this, &MainGUI::onVaultSelected);
	connect(pwGUI, &PasswordGUI::loginRequested, this, &MainGUI::onLoginRequested);
	connect(pwGUI, &PasswordGUI::backRequested, this, &MainGUI::onBackToLogin);


	/* Connect list signals */

	connect(listGUI, &ListGUI::addRequested, this, &MainGUI::onAddRequested);
	connect(listGUI, &ListGUI::editRequested, this, &MainGUI::onEditRequested);
	connect(listGUI, &ListGUI::deleteRequested, this, &MainGUI::onDeleteRequested);
	connect(listGUI, &ListGUI::saveRequested, this, &MainGUI::onSaveRequested);


	/* Connect entry dialog signals */

	connect(entryGUI, &EntryGUI::generateRequested, this, &MainGUI::onGenerateRequested);
}

MainGUI::~MainGUI() {
	;
}

void MainGUI::onVaultSelected(int mode, const QString &path) {
	pwGUI->setVaultInfo(mode, path);

	stack->setCurrentWidget(pwGUI);
}

void MainGUI::onLoginRequested(const LoginInput &input) {
	int res;


	/* Set master password */

	Password pw;
	pw.setData(input.pw);
	vault.setPW(pw);


	/* Create or open vault */

	if (input.mode == 0) {
		res = vault.newVault(input.path);
	}
	else {
		res = vault.openVault(input.path);
	}

	if (res) {
		pwGUI->setErrMsg("Failed to access vault");
		return;
	}


	/* Switch to list screen */

	vaultPath = input.path;

	refreshList();

	stack->setCurrentWidget(listGUI);
}

void MainGUI::onBackToLogin() {
	stack->setCurrentWidget(loginGUI);
}

void MainGUI::onAddRequested() {
	isEditMode = false;

	entryGUI->setAddMode();

	if (entryGUI->exec() == QDialog::Accepted) {
		Entry entry = entryGUI->getInput();

		if (vault.createEntry(entry.site, entry.acc, entry.pw)) {
			listGUI->setErrMsg("Entry already exists");
			return;
		}

		refreshList();
	}
}

void MainGUI::onEditRequested(const std::string &site, const std::string &acc) {
	isEditMode = true;
	editSite = site;
	editAcc = acc;


	/* Find the entry to get its password */

	
	Entry target = { site, acc };
	const auto &entries = vault.getEntries();
	auto it = entries.find(target);

	if (it == entries.end()) {
		listGUI->setErrMsg("Entry not found");
		return;
	}

	entryGUI->setEditMode(site, acc, it->pw);

	if (entryGUI->exec() == QDialog::Accepted) {
		Entry entry = entryGUI->getInput();
		int res = vault.updateEntry(editSite, editAcc, entry.site, entry.acc, entry.pw);

		if (res == 1) {
			listGUI->setErrMsg("Original entry not found");
			return;
		}

		if (res == 2) {
			listGUI->setErrMsg("Entry already exists");
			return;
		}

		refreshList();
	}
}

void MainGUI::onDeleteRequested(const std::string &site, const std::string &acc) {
	if (vault.deleteEntry(site, acc)) {
		listGUI->setErrMsg("Failed to delete entry");
		return;
	}

	refreshList();
}

void MainGUI::onGenerateRequested() {
	std::vector<bool> spcList = entryGUI->getSpecialsList();
	int size = entryGUI->getPasswordSize();
	Password generated;

	if (vault.genPW(generated, spcList, size)) {
		return;
	}

	entryGUI->setPassword(generated);
}

void MainGUI::onSaveRequested() {
	if (vault.saveVault(vaultPath)) {
		listGUI->setErrMsg("Failed to save vault");
		return;
	}

	listGUI->setErrMsg("Saved");
}

void MainGUI::refreshList() {
	std::vector<std::pair<std::string, std::string>> entryVec;
	const auto &entrySet = vault.getEntries();

	for (auto it = entrySet.begin(); it != entrySet.end(); it++) entryVec.emplace_back(it->site, it->acc);

	listGUI->loadEntries(entryVec);
}