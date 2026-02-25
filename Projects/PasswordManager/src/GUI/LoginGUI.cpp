/**
 * @file	LoginGUI.cpp
 * @brief	Implementation of LoginGUI class
 * @author	EilzDragon92
 */

#include "GUI/LoginGUI.h"

LoginGUI::LoginGUI(QWidget *parent) : QWidget(parent) {
	/* Create layouts and components */

	pwLine = new PWLineEdit;
	errMsg = new QLabel;
	browseBtn = new QPushButton("Browse");
	newBtn = new QPushButton("New");
	openBtn = new QPushButton("Open");
	btnBox = new QHBoxLayout;
	vBox = new QVBoxLayout;


	/* Put buttons and error message in the same line */

	btnBox->addWidget(newBtn);
	btnBox->addWidget(openBtn);
	btnBox->addWidget(errMsg);
	btnBox->addStretch();
	btnBox->setSpacing(10);
	btnBox->setContentsMargins(0, 0, 0, 0);


	/* Configure main layout */

	vBox->addWidget(browseBtn);
	vBox->addWidget(pwLine);
	vBox->addLayout(btnBox);
	vBox->addStretch();
	vBox->setSpacing(10);
	vBox->setContentsMargins(10, 10, 10, 10);

	setLayout(vBox);


	/* Connect functions to buttons */

	connect(browseBtn, &QPushButton::clicked, this, &LoginGUI::onBrowseClicked);
	connect(newBtn, &QPushButton::clicked, this, &LoginGUI::onNewClicked);
	connect(openBtn, &QPushButton::clicked, this, &LoginGUI::onOpenClicked);
}

void LoginGUI::setErrMsg(const QString &msg) {
	errMsg->setText(msg);
}