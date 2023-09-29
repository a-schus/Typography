#include "include/changepassworddialog.h"
#include "ui_changepassworddialog.h"
#include <QDebug>
#include <QMessageBox>

changePasswordDialog::changePasswordDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::changePasswordDialog)
{
	ui->setupUi(this);
}

changePasswordDialog::~changePasswordDialog()
{
	delete ui;
}

void changePasswordDialog::on_pushButton_clicked()
{
	if (!(ui->oldPass->text().size() &&
		  ui->newPass->text().size() &&
		  ui->repeatNewPass->text().size()))
	{
		QMessageBox msg;
		msg.setText("Заполните все поля.");
		msg.exec();
		return;
	}

	if (!(ui->newPass->text() == ui->repeatNewPass->text()))
	{
		QMessageBox msg;
		msg.setText("Опечатка при подтверждении пароля.");
		msg.exec();
		ui->newPass->setText("");
		ui->repeatNewPass->setText("");
		return;
	}

	QString password;
	password = user.password(user.name.fullName);

	QString enteredPass = ui->oldPass->text();
	enteredPass = QString(QCryptographicHash::hash(enteredPass.toUtf8(),
												   QCryptographicHash::Sha256).toHex());
	if (enteredPass == password)
	{
//		newPass = ui->newPass->text();
		user.setNewPassword(user.name.fullName, ui->newPass->text());
		QMessageBox msg;
		msg.setText("Успешно!\nИзменения вступят в силу при следующем входе в программу.");
		msg.exec();

		this->close();
	}
	else
	{
		QMessageBox msg;
		msg.setText("Старый пароль введен неверно.");
		msg.exec();
		ui->oldPass->setText("");
	}
}


void changePasswordDialog::on_cancelPushButton_clicked()
{
	this->close();
}

