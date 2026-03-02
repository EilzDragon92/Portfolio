/**
 * @file	EntryGUI.cpp
 * @brief	Implementation of EntryGUI class
 * @author	EilzDragon92
 */

#include "GUI/EntryGUI.h"

EntryGUI::EntryGUI(QWidget *parent) : QDialog(parent) {
	/* Create layouts and components */

	siteLine = new QLineEdit;
	accLine = new QLineEdit;
	pwLine = new PWLineEdit;
	lenSpin = new QSpinBox;
	genBtn = new QPushButton("Generate");
	okBtn = new QPushButton("OK");
	cancelBtn = new QPushButton("Cancel");
	errMsg = new QLabel;
	btnBox = new QHBoxLayout;
	spcBox = new QHBoxLayout;
	vBox = new QVBoxLayout;


	/* Set placeholder text */

	siteLine->setPlaceholderText("Site");
	accLine->setPlaceholderText("Account");


	/* Configure password length spinner */

	lenSpin->setRange(8, 128);
	lenSpin->setValue(16);
	lenSpin->setPrefix("Length: ");


	/* Configure special character checkboxes */

	for (int i = 0; i < 32; i++) {
		QString label = QString(SPC_CHARS[i]);

		spcChecks[i] = new QCheckBox(label);
		spcChecks[i]->setChecked(true);

		spcBox->addWidget(spcChecks[i]);
	}

	spcBox->addStretch();
	spcBox->setSpacing(2);
	spcBox->setContentsMargins(0, 0, 0, 0);


	/* Put OK, Cancel, and error message in the same line */

	btnBox->addWidget(okBtn);
	btnBox->addWidget(cancelBtn);
	btnBox->addWidget(errMsg);
	btnBox->addStretch();
	btnBox->setSpacing(10);
	btnBox->setContentsMargins(0, 0, 0, 0);


	/* Configure main layout */

	vBox->addWidget(siteLine);
	vBox->addWidget(accLine);
	vBox->addWidget(pwLine);
	vBox->addWidget(lenSpin);
	vBox->addLayout(spcBox);
	vBox->addWidget(genBtn);
	vBox->addLayout(btnBox);
	vBox->setSpacing(10);
	vBox->setContentsMargins(10, 10, 10, 10);

	setLayout(vBox);


	/* Connect functions to buttons */

	connect(okBtn, &QPushButton::clicked, this, &EntryGUI::onOKClicked);
	connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
	connect(genBtn, &QPushButton::clicked, this, &EntryGUI::generateRequested);
}


void EntryGUI::setAddMode() {
	setWindowTitle("Add Entry");

	siteLine->clear();
	accLine->clear();
	pwLine->clear();
	errMsg->clear();
}

void EntryGUI::setEditMode(const std::string &site, const std::string &acc, const Password &pw) {
	setWindowTitle("Edit Entry");

	siteLine->setText(QString::fromStdString(site));
	accLine->setText(QString::fromStdString(acc));
	pwLine->setPassword(pw);
	errMsg->clear();
}

EntryInput EntryGUI::getInput() {
	EntryInput input;

	input.site = siteLine->text().toStdString();
	input.acc = accLine->text().toStdString();
	pwLine->extract(input.pw);

	return input;
}

void EntryGUI::setGeneratedPW(const Password &pw) {
	pwLine->setPassword(pw);
}

std::vector<bool> EntryGUI::getSpcList() {
	std::vector<bool> list(32);

	for (int i = 0; i < 32; i++) list[i] = spcChecks[i]->isChecked();

	return list;
}

int EntryGUI::getPWLength() {
	return lenSpin->value();
}

void EntryGUI::onOKClicked() {
	errMsg->clear();

	if (siteLine->text().isEmpty()) {
		errMsg->setText("Site is not input");
		return;
	}

	if (accLine->text().isEmpty()) {
		errMsg->setText("Account is not input");
		return;
	}

	Password tmp;
	pwLine->extract(tmp);

	if (tmp.isEmpty()) {
		errMsg->setText("Password is not input");
		return;
	}

	pwLine->setPassword(tmp);

	accept();
}