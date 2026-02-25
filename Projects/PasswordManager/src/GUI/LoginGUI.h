/**
 * @file	LoginGUI.h
 * @brief	Login window for vault management
 * @author	EilzDragon92
 */

#include "Common/header.h"
#include "GUI/PWLineEdit.h"

/**
 * @struct	LoginInput
 * @brief	Container for login input parameters
 */
struct LoginInput {
	QString path;
	Password pw;
};

/**
 * @class	LoginGUI
 * @brief	Login window for vault management
 */
class LoginGUI : public QWidget {
	Q_OBJECT

public:
	/**
	 * @brief	Constructor of LoginGUI class
	 * @param	parent	Parent widget
	 */
	explicit LoginGUI(QWidget *parent = nullptr);

	/**
	 * @brief	Display error message
	 * @param	msg		Error message string
	 */
	void setErrMsg(const QString &msg);

private slots:
	/**
	 * @brief	Open file dialog to select vault file
	 */
	void onBrowseClicked();

	/**
	 * @brief	Validate input and emit new vault request
	 */
	void onNewClicked();

	/**
	 * @brief	Validate input and emit open vault request
	 */
	void onOpenClicked();

private:
	PWLineEdit *pwLine;
	QLabel *errMsg;
	QPushButton *browseBtn;
	QPushButton *newBtn;
	QPushButton *openBtn;
	QHBoxLayout *btnBox;
	QVBoxLayout *vBox;
};