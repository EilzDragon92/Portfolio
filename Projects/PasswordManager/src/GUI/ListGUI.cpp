/**
 * @file	ListGUI.cpp
 * @brief	Implementation of ListGUI class
 * @author	EilzDragon92
 */

#include "GUI/ListGUI.h"

ListGUI::ListGUI(QWidget *parent) : QWidget(parent) {
	/* Create layouts and components */
	
	errMsg = new QLabel;
	searchLine = new QLineEdit;
	addBtn = new QPushButton("Add");
	editBtn = new QPushButton("Edit");
	deleteBtn = new QPushButton("Delete");
	copyPWBtn = new QPushButton("Copy PW");
	saveBtn = new QPushButton("Save");
	table = new QTableWidget;
	btnBox = new QHBoxLayout;
	vBox = new QVBoxLayout;


	/* Configure search bar */

	searchLine->setPlaceholderText("Search");


	/* Configure table */

	table->setColumnCount(2);
	table->setHorizontalHeaderLabels({ "Site", "Account" });
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setSelectionMode(QAbstractItemView::SingleSelection);
	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	table->horizontalHeader()->setStretchLastSection(true);
	table->verticalHeader()->setVisible(false);


	/* Put buttons and error message in the same line */

	btnBox->addWidget(addBtn);
	btnBox->addWidget(editBtn);
	btnBox->addWidget(deleteBtn);
	btnBox->addWidget(copyPWBtn);
	btnBox->addWidget(saveBtn);
	btnBox->addWidget(errMsg);
	btnBox->addStretch();
	btnBox->setSpacing(10);
	btnBox->setContentsMargins(0, 0, 0, 0);


	/* Configure main layout */

	vBox->addWidget(searchLine);
	vBox->addWidget(table);
	vBox->addLayout(btnBox);
	vBox->setSpacing(10);
	vBox->setContentsMargins(10, 10, 10, 10);

	setLayout(vBox);


	/* Connect functions to buttons */

	connect(addBtn, &QPushButton::clicked, this, &ListGUI::onAddClicked);
	connect(editBtn, &QPushButton::clicked, this, &ListGUI::onEditClicked);
	connect(deleteBtn, &QPushButton::clicked, this, &ListGUI::onDeleteClicked);
	connect(copyPWBtn, &QPushButton::clicked, this, &ListGUI::onCopyPWClicked);
	connect(saveBtn, &QPushButton::clicked, this, &ListGUI::saveRequested);
	connect(searchLine, &QLineEdit::textChanged, this, &ListGUI::onSearchChanged);
}

void ListGUI::loadEntries(const std::vector<std::pair<std::string, std::string>> &entries) {
	table->setRowCount(0);

	for (const auto &[site, acc] : entries) {
		int row = table->rowCount();

		table->insertRow(row);
		table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(site)));
		table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(acc)));
	}

	errMsg->clear();
}

void ListGUI::setErrMsg(const QString &msg) {
	errMsg->setText(msg);
}

void ListGUI::onAddClicked() {
	errMsg->clear();

	emit addRequested();
}

void ListGUI::onEditClicked() {
	std::string site, acc;

	if (!getSelectedEntry(site, acc)) return;

	emit editRequested(site, acc);
}

void ListGUI::onDeleteClicked() {
	std::string site, acc;

	if (!getSelectedEntry(site, acc)) return;

	emit deleteRequested(site, acc);
}

void ListGUI::onCopyPWClicked() {
	std::string site, acc;

	if (!getSelectedEntry(site, acc)) return;

	emit copyPWRequested(site, acc);
}

void ListGUI::onSearchChanged(const QString &text) {
	for (int i = 0; i < table->rowCount(); i++) {
		bool match = false;

		for (int j = 0; j < table->columnCount(); j++) {
			QTableWidgetItem *item = table->item(i, j);

			if (item && item->text().contains(text, Qt::CaseInsensitive)) {
				match = true;
				break;
			}
		}

		table->setRowHidden(i, !match);
	}
}

bool ListGUI::getSelectedEntry(std::string &site, std::string &acc) {
	int row = table->currentRow();

	if (row < 0) {
		errMsg->setText("No entry selected");
		return false;
	}

	errMsg->clear();

	site = table->item(row, 0)->text().toStdString();
	acc = table->item(row, 1)->text().toStdString();

	return true;
}