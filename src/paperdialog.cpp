#include "include/paperdialog.h"
#include "ui_paperdialog.h"

#include <QMessageBox>

PaperDialog::PaperDialog(QSqlTableModel *papersModel, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PaperDialog),
	model(papersModel)
{
	ui->setupUi(this);

	paperTypeModel = new QSqlTableModel(this, db);
	paperTypeModel->setEditStrategy(QSqlTableModel::EditStrategy::OnManualSubmit);
	paperTypeModel->setTable("paper_types");
	paperTypeModel->sort(tPaperTypes::type, Qt::SortOrder::AscendingOrder);
	paperTypeModel->select();
	ui->paperTypeComboBox->setModel(paperTypeModel);
	ui->paperTypeComboBox->setModelColumn(tPaperTypes::type);

	printModel = new QSqlTableModel(this, db);
	printModel->setTable("prints");
	printModel->select();
	ui->printComboBox->setModel(printModel);
	ui->printComboBox->setModelColumn(tPrints::print);

	papersMapper = new QDataWidgetMapper(this);
	papersMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
	papersMapper->setModel(model);

	papersMapper->addMapping(ui->paperTypeComboBox, tPapers::paperType);
	papersMapper->addMapping(ui->paperNameLineEdit, tPapers::paperName);
	papersMapper->addMapping(ui->xSpinBox, tPapers::x);
	papersMapper->addMapping(ui->ySpinBox, tPapers::y);
	papersMapper->addMapping(ui->densitySpinBox, tPapers::density);
	papersMapper->addMapping(ui->printComboBox, tPapers::print);
	papersMapper->addMapping(ui->priceDoubleSpinBox, tPapers::price);
}

PaperDialog::~PaperDialog()
{
	delete ui;
}

NewPaperDialog::NewPaperDialog(QSqlTableModel *papersModel, int curId, QWidget *parent) :
	PaperDialog(papersModel, parent),
	currentId(curId)
{

}

NewPaperDialog::~NewPaperDialog()
{

}

void NewPaperDialog::newPaperPrep()
{
	ui->paperTypeComboBox->setCurrentIndex(-1);
	ui->printComboBox->setCurrentIndex(-1);
}

EditPaperDialog::EditPaperDialog(QSqlTableModel *papersModel, int curId, QWidget *parent) :
	PaperDialog(papersModel, parent),
	currentId(curId)
{

}

EditPaperDialog::~EditPaperDialog()
{

}

void PaperDialog::on_printComboBox_currentIndexChanged(int index)
{
	if (index < 0)
	{
		ui->xSpinBox->setEnabled(false);
		ui->ySpinBox->setEnabled(false);
	}
	else
	{
		ui->xSpinBox->setEnabled(true);
		ui->ySpinBox->setEnabled(true);
	}
}

void NewPaperDialog::on_okPushButton_clicked()
{
	if (ui->paperTypeComboBox->currentIndex() < 0
		|| !ui->paperNameLineEdit->text().size()
		|| ui->printComboBox->currentIndex() < 0
		|| !ui->priceDoubleSpinBox->value()
		|| !ui->xSpinBox->value()
		|| !ui->ySpinBox->value()
		|| !ui->densitySpinBox->value())
	{
		QMessageBox msg;
		msg.setText("Заполните все поля.");
		msg.exec();
		return;
	}

	settings::WorkTypes wt = getPrintParams(ui->printComboBox->currentText());
	int x = ui->xSpinBox->value();
	int y = ui->ySpinBox->value();
	if (!((x <= wt.maxPaperHeight && y <= wt.maxPaperWidth) ||
		  (y <= wt.maxPaperHeight && x <= wt.maxPaperWidth)))
	{
		QMessageBox msg;
		msg.setText("Размер бумаги не соответствует максимальным параметрам машины.");
		msg.exec();
		return;
	}

	papersMapper->submit();
	model->select();
	emit refreshPaperTable(maxId(model, tPapers::id));
	this->close();
}

void NewPaperDialog::on_cancelPushButton_clicked()
{
	emit refreshPaperTable(curRow(currentId, model, tPapers::id));
	this->close();
}

void EditPaperDialog::on_okPushButton_clicked()
{
	if (ui->paperTypeComboBox->currentIndex() < 0
		|| !ui->paperNameLineEdit->text().size()
		|| ui->printComboBox->currentIndex() < 0
		|| !ui->priceDoubleSpinBox->value()
		|| !ui->xSpinBox->value()
		|| !ui->ySpinBox->value()
		|| !ui->densitySpinBox->value())
	{
		QMessageBox msg;
		msg.setText("Заполните все поля.");
		msg.exec();
		return;
	}

	settings::WorkTypes wt = getPrintParams(ui->printComboBox->currentText());
	int x = ui->xSpinBox->value();
	int y = ui->ySpinBox->value();
	if (!((x <= wt.maxPaperHeight && y <= wt.maxPaperWidth) ||
		  (y <= wt.maxPaperHeight && x <= wt.maxPaperWidth)))
	{
		QMessageBox msg;
		msg.setText("Размер бумаги не соответствует максимальным параметрам машины.");
		msg.exec();
		return;
	}

	papersMapper->submit();
	model->select();
	emit refreshPaperTable(curRow(currentId, model, tPapers::id));
	this->close();
}

void EditPaperDialog::on_cancelPushButton_clicked()
{
	emit refreshPaperTable(curRow(currentId, model, tPapers::id));
	this->close();
}

void PaperDialog::on_toolButton_clicked()
{
	NewPaperTypeDialog *newPaperType = new NewPaperTypeDialog(paperTypeModel, this);
	connect(newPaperType, SIGNAL(resetComboBox()),
			this, SLOT(resetPaperTypesComboBox()));
	newPaperType->exec();
}

void PaperDialog::resetPaperTypesComboBox()
{
	QString type = ui->paperTypeComboBox->currentText();
	paperTypeModel->submitAll();
	paperTypeModel->sort(tPaperTypes::type, Qt::SortOrder::AscendingOrder);
	int row = ui->paperTypeComboBox->findText(type);
	ui->paperTypeComboBox->setCurrentIndex(row);
}

