#include "include/clientdialog.h"
#include "ui_clientdialog.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QKeyEvent>


ClientDialog::ClientDialog(QSqlTableModel *clientModel, int curId, bool isNew, QWidget *parent) :
    QDialog(parent),
	ui(new Ui::ClientDialog),
	model(clientModel),
	currentId(curId)
{
	ClientDialog::isNew = isNew;
	ui->setupUi(this);

    clientMapper = new QDataWidgetMapper(this);
    clientMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
	clientMapper->setModel(model);

	clientMapper->addMapping(ui->company, 1);
	clientMapper->addMapping(ui->person, 2);
	clientMapper->addMapping(ui->eMail, 3);
	clientMapper->addMapping(ui->address,4);
	clientMapper->addMapping(ui->inn, 5);
	clientMapper->addMapping(ui->type, 6, "currentIndex");
	clientMapper->addMapping(ui->phone, 7);
	clientMapper->addMapping(ui->discoun, 8);
	clientMapper->addMapping(ui->credit, 9);
	clientMapper->addMapping(ui->duty, 10);
	clientMapper->addMapping(ui->companyPhone, tClients::companyPhone);
}

ClientDialog::~ClientDialog()
{
    delete ui;
}

void ClientDialog::on_saveNewButton_clicked()
{
    if (!ui->person->text().size())
    {
        QMessageBox msg;
        msg.setText("Заполните поле \"Представитель\"!");
        msg.exec();
        return;
    }
	if (!ui->phone->text().size() && !ui->companyPhone->text().size())
    {
        QMessageBox msg;
        msg.setText("Заполните поле \"Телефон\"!");
        msg.exec();
        return;
    }
	clientMapper->submit();
	model->select();
	if (isNew)
		emit refreshClientSelectDialog(maxId(model));
	else
	{
		emit refreshClientSelectDialog(curRow(currentId, model));
	}

    this->close();
}


void ClientDialog::on_cancelButton_clicked()
{
	emit refreshClientSelectDialog(curRow(currentId, model));
    this->close();
}

void ClientDialog::keyPressEvent(QKeyEvent *event)
{
	qDebug() << event->key();
	if ((event->key() == Qt::Key::Key_Enter) || (event->key() == Qt::Key::Key_Return))
		on_saveNewButton_clicked();
	if (event->key() == Qt::Key::Key_Escape)
		on_cancelButton_clicked();

	QDialog::keyReleaseEvent(event);
}
