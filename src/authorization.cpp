#include "include/authorization.h"
#include "ui_authorization.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QSqlError>


Authorization::Authorization(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Authorization)
{
	ui->setupUi(this);
	ui->newUserPushButton->hide();

	userModel = new QSqlTableModel(this, db);
	userModel->setTable("users");
	userModel->setEditStrategy(QSqlTableModel::EditStrategy::OnFieldChange);
	userModel->sort(tUsers::full_name, Qt::SortOrder::AscendingOrder);
	userModel->select();
	ui->userNameComboBox->setModel(userModel);
	ui->userNameComboBox->setModelColumn(tUsers::full_name);
	ui->userNameComboBox->setCurrentIndex(-1);
}

Authorization::~Authorization()
{
	delete ui;
}

void Authorization::on_exitPushButton_clicked()
{
	std::exit(0);
}

void Authorization::closeEvent(QCloseEvent *event)
{
	std::exit(0);
}

void Authorization::on_okPushButton_clicked()
{
	QModelIndex idx = userModel->index(ui->userNameComboBox->currentIndex(),
								  tUsers::password, QModelIndex());
	QString password = userModel->data(idx).toString();

	QString enteredPass = ui->passwordLineEdit->text();
	enteredPass = QString(QCryptographicHash::hash(enteredPass.toUtf8(),
												   QCryptographicHash::Sha256).toHex());

	if (enteredPass == password)
	{
		user.name.fullName = userModel->data(userModel->index(ui->userNameComboBox->currentIndex(),
															  tUsers::full_name)).toString();
		user.name.printName = userModel->data(userModel->index(ui->userNameComboBox->currentIndex(),
															  tUsers::print_name)).toString();
		this->done(QDialog::Accepted);
	}
	else
	{
		QMessageBox msg;
		msg.setText("Пароль неверный!");
		msg.exec();
	}
}


void Authorization::on_newUserPushButton_clicked()
{
	NewUserDialog* newUser = new NewUserDialog(userModel, this);
	newUser->setAttribute(Qt::WA_DeleteOnClose);
	newUser->exec();
}


void Authorization::on_userNameComboBox_currentIndexChanged(int index)
{
	if (index < 0)
	{
		ui->passwordLineEdit->setEnabled(false);
		ui->okPushButton->setEnabled(false);
	}
	else
	{
		ui->passwordLineEdit->setEnabled(true);
		ui->okPushButton->setEnabled(true);
		ui->passwordLineEdit->setFocus();
	}
}


void Authorization::reject()
{
	std::exit(0);
}


void Authorization::keyPressEvent(QKeyEvent *event)
{
	if ((event->key() == Qt::Key::Key_Enter) || (event->key() == Qt::Key::Key_Return))
		on_okPushButton_clicked();

	QDialog::keyPressEvent(event);
}
