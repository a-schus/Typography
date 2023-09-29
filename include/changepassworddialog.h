#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include "tuser.h"

#include <QDialog>
#include <QString>
#include <QCryptographicHash>

extern TUser user;

namespace Ui {
	class changePasswordDialog;
}

class changePasswordDialog : public QDialog
{
	Q_OBJECT

public:
    explicit changePasswordDialog(QWidget *parent = nullptr);
	~changePasswordDialog();

private:
	Ui::changePasswordDialog *ui;

	// QWidget interface
protected:
private slots:
	void on_pushButton_clicked();
	void on_cancelPushButton_clicked();
};

#endif // CHANGEPASSWORDDIALOG_H
