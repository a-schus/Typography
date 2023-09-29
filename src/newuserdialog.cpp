#include "include/newuserdialog.h"
#include "ui_newuserdialog.h"

#include <QMessageBox>
#include <QSqlError>


NewUserDialog::NewUserDialog(QSqlTableModel* model, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::NewUserDialog),
	userModel(model)
{
	ui->setupUi(this);

}

NewUserDialog::~NewUserDialog()
{
	delete ui;
}

void NewUserDialog::on_cancelPushButton_clicked()
{
	this->close();
}


void NewUserDialog::on_okPushButton_clicked()
{
	if (ui->fullNameLineEdit->text().size() &&
        ui->printNameLineEdit->text().size() &&
		ui->passwordLineEdit->text().size() &&
        ui->repeatPasswordLineEdit->text().size())
	{
		if (isUserExists(ui->fullNameLineEdit->text()))
		{
			QMessageBox msg;
			msg.setText("Такой пользователь уже существует.");
			msg.exec();
			return;
		}
		if (!(ui->passwordLineEdit->text() == ui->repeatPasswordLineEdit->text()))
			return;

		QSqlTableModel::EditStrategy userModelStrategy = userModel->editStrategy();

		userModel->setEditStrategy(QSqlTableModel::EditStrategy::OnManualSubmit);
		int row = userModel->rowCount();
		userModel->insertRow(row);
		userModel->setData(userModel->index(row, tUsers::full_name), ui->fullNameLineEdit->text());
		userModel->setData(userModel->index(row, tUsers::print_name), ui->printNameLineEdit->text());
		QString password = ui->passwordLineEdit->text();
		password = QString(QCryptographicHash::hash(password.toUtf8(),
													QCryptographicHash::Sha256).toHex());
		userModel->setData(userModel->index(row, tUsers::password), password);
		userModel->setData(userModel->index(row, tUsers::createUsers),
						   ui->hasRightCreateUsers->isChecked());

		userModel->submitAll();

		userModel->setEditStrategy(userModelStrategy);


		this->close();
	}
	else
	{
		QMessageBox msg;
		msg.setText("Все поля должны быть заполнены.");
		msg.exec();
	}
}

bool NewUserDialog::isUserExists(QString fullName)
{
	for (int i = 0; i < userModel->rowCount(); ++i)
	{
		if (userModel->data(userModel->index(i, tUsers::full_name)).toString() == fullName)
			return true;
	}
	return false;
}



int NewUserDialog::exec()
{
    if (!user.hasRightCreateUsers())
    {
        QMessageBox msg;
        msg.setText("У вас нет прав на совершение данной операции.");
        msg.exec();
        return 1;
    }
	QDialog::exec();
    return 0;
}
