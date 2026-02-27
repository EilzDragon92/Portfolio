/**
 * @file	ListGUI.h
 * @brief	Entry list window with CRUD operations
 * @author	EilzDragon92
 */

#pragma once

#include "Common/header.h"

/**
 * @class	ListGUI
 * @brief	Entry list window with CRUD operations
 */
class ListGUI : public QWidget {
	Q_OBJECT

public:
	/**
	 * @brief	Constructor of ListGUI class
	 * @param	parent	Parent widget
	 */
	explicit ListGUI(QWidget *parent = nullptr);

	/**
	 * @brief	Refresh the table with current entry data
	 * @param	entries		List of entries as (site, account) pairs
	 */
	void loadEntries(const std::vector<std::pair<std::string, std::string>> &entries);

	/**
	 * @brief	Display error message
	 * @param	msg		Error message string
	 */
	void setErrMsg(const QString &msg);

signals:
	/**
	 * @brief	Signal when add button is clicked
	 */
	void addRequested();

	/**
	 * @brief	Signal when edit button is clicked
	 * @param	site	Site name of the selected entry
	 * @param	acc		Account of the selected entry
	 */
	void editRequested(const std::string &site, const std::string &acc);

	/**
	 * @brief	Signal when delete button is clicked
	 * @param	site	Site name of the selected entry
	 * @param	acc		Account of the selected entry
	 */
	void deleteRequested(const std::string &site, const std::string &acc);

	/**
	 * @brief	Signal when copy password button is clicked
	 * @param	site	Site name of the selected entry
	 * @param	acc		Account of the selected entry
	 */
	void copyPWRequested(const std::string &site, const std::string &acc);

	/**
	 * @brief	Signal when save button is clicked
	 */
	void saveRequested();

private slots:
	/**
	 * @brief	Handle add button click
	 */
	void onAddClicked();

	/**
	 * @brief	Handle edit button click
	 */
	void onEditClicked();

	/**
	 * @brief	Handle delete button click
	 */
	void onDeleteClicked();

	/**
	 * @brief	Handle copy password button click
	 */
	void onCopyPWClicked();

	/**
	 * @brief	Filter table rows based on search text
	 * @param	text	Search text
	 */
	void onSearchChanged(const QString &text);

private:
	QLabel *errMsg;
	QLineEdit *searchLine;
	QPushButton *addBtn;
	QPushButton *editBtn;
	QPushButton *deleteBtn;
	QPushButton *copyPWBtn;
	QPushButton *saveBtn;
	QTableWidget *table;
	QHBoxLayout *btnBox;
	QVBoxLayout *vBox;

	/**
	 * @brief	Get site and account from the selected row
	 * @param	site	Destination for site name
	 * @param	acc		Destination for account
	 * @return	true if a row is selected
	 */
	bool getSelectedEntry(std::string &site, std::string &acc);
};