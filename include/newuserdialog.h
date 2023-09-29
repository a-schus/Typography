#ifndef NEWUSERDIALOG_H
#define NEWUSERDIALOG_H

#include <QDialog>
#include <QSqlTableModel>
#include <QCryptographicHash>

#include "typography.h"

namespace Ui {
	class NewUserDialog;
}

class NewUserDialog : public QDialog
{
	Q_OBJECT

public:
	explicit NewUserDialog(QSqlTableModel* model, QWidget *parent = nullptr);
	~NewUserDialog();

private slots:
	void on_cancelPushButton_clicked();

	void on_okPushButton_clicked();

private:
	bool isUserExists(QString fullName);

private:
	Ui::NewUserDialog *ui;
	QSqlTableModel* userModel;

	// QDialog interface
public slots:
	virtual int exec() override;
};

#endif // NEWUSERDIALOG_H
