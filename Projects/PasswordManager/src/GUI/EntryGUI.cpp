/**
 * @file	EntryGUI.cpp
 * @brief	Implementation of EntryGUI class
 * @author	EilzDragon92
 */

#include "GUI/EntryGUI.h"

EntryGUI::EntryGUI(QWidget *parent) : QDialog(parent) {
	/* Create layouts and components */

	pwLine = new PWLineEdit;
	spcGrid = new QGridLayout;
	errMsg = new QLabel;
	lenLabel = new QLabel("Length: 16");
	siteLine = new QLineEdit;
	accLine = new QLineEdit;
	genBtn = new QPushButton("Generate");
	okBtn = new QPushButton("OK");
	lenSlider = new QSlider(Qt::Horizontal);
	cancelBtn = new QPushButton("Cancel");
	btnBox = new QHBoxLayout;
	lenBox = new QHBoxLayout;
	vBox = new QVBoxLayout;


	/* Set placeholder text */

	siteLine->setPlaceholderText("Site");
	accLine->setPlaceholderText("Account");


	/* Configure password length slider */

	lenSlider->setRange(8, 32);
	lenSlider->setValue(16);

	lenBox->addWidget(lenLabel);
	lenBox->addWidget(lenSlider);
	lenBox->setSpacing(10);
	lenBox->setContentsMargins(0, 0, 0, 0);


	/* Configure special character checkboxes */

	for (int i = 0; i < 32; i++) {
		QString label = QString(spcs[i]);

		spcChecks[i] = new QCheckBox(label);
		spcChecks[i]->setChecked(true);

		spcGrid->addWidget(spcChecks[i], i / 8, i % 8);
	}

	spcGrid->setSpacing(5);
	spcGrid->setContentsMargins(0, 0, 0, 0);


	/* Put OK, Cancel, and error message in the same line */

	btnBox->addWidget(genBtn);
	btnBox->addWidget(okBtn);
	btnBox->addWidget(cancelBtn);
	btnBox->addStretch();

	btnBox->setSpacing(10);
	btnBox->setContentsMargins(0, 0, 0, 0);


	/* Configure main layout */

	vBox->addWidget(siteLine);
	vBox->addWidget(accLine);
	vBox->addWidget(pwLine);
	vBox->addLayout(lenBox);
	vBox->addLayout(spcGrid);
	vBox->addWidget(errMsg);
	vBox->addLayout(btnBox);

	vBox->setSpacing(10);
	vBox->setContentsMargins(10, 10, 10, 10);

	setLayout(vBox);


	/* Connect functions to buttons */

	connect(okBtn, &QPushButton::clicked, this, &EntryGUI::onOKClicked);
	connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
	connect(genBtn, &QPushButton::clicked, this, &EntryGUI::generateRequested);

	connect(lenSlider, &QSlider::valueChanged, this, [this](int val) {
		lenLabel->setText(QString("Length: %1").arg(val));
	});
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

Entry EntryGUI::getInput() {
	Entry entry;

	entry.site = siteLine->text().toStdString();
	entry.acc = accLine->text().toStdString();
	pwLine->extract(entry.pw);

	return entry;
}

void EntryGUI::setPassword(const Password &pw) {
	pwLine->setPassword(pw);
}

std::vector<bool> EntryGUI::getSpecialsList() {
	std::vector<bool> list(32);

	for (int i = 0; i < 32; i++) list[i] = spcChecks[i]->isChecked();

	return list;
}

int EntryGUI::getPasswordSize() {
	return lenSlider->value();
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