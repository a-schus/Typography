#include "include/pasportdistributionwidget.h"
#include "ui_pasportdistributionwidget.h"

#include <QMessageBox>

PasportDistributionWidget::PasportDistributionWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PasportDistributionWidget)
{
	ui->setupUi(this);

	// Рисуем на кнопках стрелку вверх и стрелку внпиз
	ui->toolButton->setText(QString("Поставить в печать ") + QChar(0x02C5));
	ui->toolButton_2->setText(QChar(0x02C4) + QString(" Снять с печати"));

	currentDatePasportTable = new PasportBaseViewWidget(ui->currentDateWidget->selectedDate(),
														newOrPre::newPasport,
														parent);

	currentDatePasportTable->setAttribute(Qt::WA_DeleteOnClose);

	layout_1 = new QVBoxLayout;
	layout_1->addWidget(currentDatePasportTable);
	ui->frame->setLayout(layout_1);

	pasportTable = new PasportBaseViewWidget(newOrPre::newPasport, parent);
	pasportTable->setAttribute(Qt::WA_DeleteOnClose);
	QObject::connect(this, &PasportDistributionWidget::closeAll,
					 pasportTable, &PasportBaseViewWidget::closeAll);

	// соединяем сигналы на обновление обеих таблиц паспортов-заказов
	// со слотами обновления в каждой таблице
	QObject::connect(currentDatePasportTable, &PasportBaseViewWidget::refreshAll,
					 currentDatePasportTable, &PasportBaseViewWidget::on_refreshPushButton_clicked);
	QObject::connect(pasportTable, &PasportBaseViewWidget::refreshAll,
					 currentDatePasportTable, &PasportBaseViewWidget::on_refreshPushButton_clicked);
	QObject::connect(pasportTable, &PasportBaseViewWidget::refreshAll,
					 pasportTable, &PasportBaseViewWidget::on_refreshPushButton_clicked);
	QObject::connect(currentDatePasportTable, &PasportBaseViewWidget::refreshAll,
					 pasportTable, &PasportBaseViewWidget::on_refreshPushButton_clicked);

	// сигнал для обновления таблицы паспортов-заказов
	// при создании и сохранении нового заказа
	QObject::connect(this, &PasportDistributionWidget::refreshTable,
					 pasportTable, &PasportBaseViewWidget::refreshTable);

	layout_2 = new QVBoxLayout;
	layout_2->addWidget(pasportTable);
	ui->frame_2->setLayout(layout_2);
}

PasportDistributionWidget::~PasportDistributionWidget()
{
	delete ui;
}

void PasportDistributionWidget::on_toolButton_clicked()
{
	QDate date = ui->currentDateWidget->selectedDate();
	if (date < QDate::currentDate())
	{
		QMessageBox msg;
		msg.setText("Невозможно поставить заказ в печать на прошедший день.");
		msg.setIcon(QMessageBox::Warning);
		msg.exec();
		return;
	}
	QModelIndex idx = pasportTable->curIndex();
	idx = pasportTable->pasportOfsetModel->mapToSource(idx);
	idx = pasportTable->pasportOfsetSourseModel->index(idx.row(), tPasportOfset::printDate);
	if (pasportTable->
		pasportOfsetSourseModel->
		data(pasportTable->pasportOfsetSourseModel->index(idx.row(), tPasportOfset::deleted)).toBool())
	{
		QMessageBox msg;
		msg.setText("Невозможно поставить в печать удаленный заказ.");
		msg.setIcon(QMessageBox::Warning);
		msg.exec();
		return;
	}

	pasportTable->pasportOfsetSourseModel->setData(idx, date);

	idx = pasportTable->pasportOfsetSourseModel->index(idx.row(), tPasportOfset::id);
	currentDatePasportTable->refreshTableAfterEdit(pasportTable->pasportOfsetSourseModel->
												   data(idx).toLongLong());
}


void PasportDistributionWidget::on_currentDateWidget_clicked(const QDate &date)
{
	currentDatePasportTable->pasportOfsetSourseModel->setFilter("print_date = '" + QVariant(date).toString() + "'");
}


void PasportDistributionWidget::on_toolButton_2_clicked()
{
	QModelIndex idx = currentDatePasportTable->curIndex();
	idx = currentDatePasportTable->pasportOfsetModel->mapToSource(idx);
	idx = currentDatePasportTable->pasportOfsetSourseModel->index(idx.row(), tPasportOfset::printDate);
	currentDatePasportTable->pasportOfsetSourseModel->setData(idx, QVariant(0).toDate());

	currentDatePasportTable->refreshTable();
	pasportTable->on_refreshPushButton_clicked();
}

void PasportDistributionWidget::on_toolButton_3_clicked()
{
	if (ui->widget->isHidden())
	{
		ui->widget->show();
		ui->toolButton_3->setText("-");
		ui->toolButton->show();
		ui->toolButton_2->show();
	}
	else
	{
		ui->widget->hide();
		ui->toolButton_3->setText("+");
		ui->toolButton->hide();
		ui->toolButton_2->hide();
	}

}

