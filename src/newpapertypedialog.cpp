#include "include/newpapertypedialog.h"
#include "ui_newpapertypedialog.h"

#include <QSqlRecord>

NewPaperTypeDialog::NewPaperTypeDialog(QSqlTableModel *model, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::NewPaperTypeDialog),
	paperTypesModel(model)
{
	ui->setupUi(this);
}

NewPaperTypeDialog::~NewPaperTypeDialog()
{
	delete ui;
}

void NewPaperTypeDialog::on_cancelPushButton_clicked()
{
	this->close();
}


void NewPaperTypeDialog::on_okPushButton_clicked()
{
	int row = paperTypesModel->rowCount();
	paperTypesModel->insertRows(row, 1);

	QSqlRecord record = paperTypesModel->record(row);
	record.setValue(tPaperTypes::type, ui->nameLineEdit->text());
	record.setValue(tPaperTypes::coefficient, 1);
	record.setGenerated(tPaperTypes::type, true);
	record.setGenerated(tPaperTypes::coefficient, true);
	paperTypesModel->setRecord(row, record);

	emit resetComboBox();

	this->close();
}

