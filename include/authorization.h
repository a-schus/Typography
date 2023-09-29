#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QModelIndex>
#include "typography.h"
#include "changepassworddialog.h"
#include "newuserdialog.h"
#include "tuser.h"

extern QSqlDatabase db;
extern TUser user;



namespace Ui {
	class Authorization;
}

class Authorization : public QDialog
{
	Q_OBJECT

public:
	explicit Authorization(QWidget *parent = nullptr);
	~Authorization();

private slots:
	void on_exitPushButton_clicked();

	void on_okPushButton_clicked();

	void on_newUserPushButton_clicked();

	void on_userNameComboBox_currentIndexChanged(int index);

private:
	Ui::Authorization *ui;

	QSqlTableModel* userModel;

	// QWidget interface
protected:
	virtual void closeEvent(QCloseEvent *event) override;

	// QDialog interface
public slots:
	virtual void reject() override; // чтобы при нажатии "Esc" закрывалась программа, а не только окно авторизации

	// QWidget interface
protected:
	virtual void keyPressEvent(QKeyEvent *event) override;
};

#endif // AUTHORIZATION_H
