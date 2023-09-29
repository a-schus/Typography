#include "include/mainwindow.h"
#include "include/pasportbaseviewwidget.h"
#include "ui_pasportbaseviewwidget.h"

#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlError>
#include <QDateEdit>
#include <QFile>
#include <QPrintDialog>


// -----------------------------------------------------------------------------------------------
// TPasportModel
// -----------------------------------------------------------------------------------------------

TPasportModel::TPasportModel(int pre, QWidget *parent, const QSqlDatabase &db) :
	QSortFilterProxyModel(parent/*, db*/),
	preOrNew(pre)
{

}

QVariant TPasportModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

//	перечеркиваем удаленные заказы
//	if (role == Qt::FontRole)
//		if (QSortFilterProxyModel::data(this->index(index.row(), tPasportOfset::deleted)).toBool())
//		{
//			QFont font = QSortFilterProxyModel::data(index, Qt::FontRole).value<QFont>();
//			font.setStrikeOut(true);
//			return font;
//		}

	if (role == Qt::ForegroundRole)
		if (QSortFilterProxyModel::data(this->index(index.row(), tPasportOfset::deleted)).toBool())
			return QBrush(QColor(128, 128, 128));
	if (role == Qt::BackgroundRole)
		if (QSortFilterProxyModel::data(this->index(index.row(), tPasportOfset::deleted)).toBool())
		{
			if (!(index.row() % 2))
				return QBrush(QColor(255, 235, 235));
			else return QBrush(QColor(255, 225, 225));
		}
	if (role == Qt::TextAlignmentRole)
		if (index.column() == tPasportOfset::paid ||
			index.column() == tPasportOfset::printed ||
			index.column() == tPasportOfset::prepared ||
			index.column() == tPasportOfset::issued)
			return Qt::AlignCenter
					;

	if (role == Qt::DisplayRole)
	{
		if (index.column() == tPasportOfset::id)
			if (QSortFilterProxyModel::data(index).toLongLong() < 0)
				return -QSortFilterProxyModel::data(index).toLongLong();
		if (index.column() == tPasportOfset::paid ||
			index.column() == tPasportOfset::printed ||
			index.column() == tPasportOfset::prepared ||
			index.column() == tPasportOfset::issued)
		{
			if (sourceModel()->data(QSortFilterProxyModel::mapToSource(index)).toBool())
				return QChar(0x2705);
			else return "-";
		}
		if (index.column() == tPasportOfset::cost)
			return tlib::conformCostString(QSortFilterProxyModel::data(index).toDouble());
		return QSortFilterProxyModel::data(index);
	}

	if (role == Qt::EditRole)
		return QSortFilterProxyModel::data(index);

	return QVariant();
}

void TPasportModel::addFilter(const FilterItem &filterItem)
{
	if (filterItem.filterOperator == AND)
		filterList.append(filterItem);
	else
		filterList.prepend(filterItem);

	invalidate();
}

void TPasportModel::eraseFilter(const FilterItem &filterItem)
{
	for (int i = 0; i < filterList.size(); ++i)
		if ((filterList[i].filterString == filterItem.filterString) &&
			(filterList[i].column) == filterItem.column)
			filterList.erase(filterList.begin() + i);
	invalidate();
}

bool TPasportModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	// Если ID отрицательный, то это предварительный расчет,
	// в списке паспортов-заказов не отображается
	switch (preOrNew)
	{
		case newOrPre::newPasport:
			if (sourceModel()->index(source_row, tPasportOfset::id, source_parent).data().toInt() <= 0)
				return false;
			break;
		case newOrPre::prePasport:
			if (sourceModel()->index(source_row, tPasportOfset::id, source_parent).data().toInt() >= 0)
				return false;
			break;
		default: break;
	}
	//------------------------------------------------------

	bool res = true;
	int i = 0;
	foreach (FilterItem item, filterList)
	{
		bool tempRes = true;
		if (i++ == 0)
			item.filterOperator = AND;

		const QVariant& data = sourceModel()->
							   index(source_row,
									 item.column,
									 source_parent).
							   data();
		switch (item.condition)
		{
			case Equal:
				tempRes = (data.toString() == item.filterString);
				break;
			case NotEqual:
				tempRes = (data.toString() != item.filterString);
				break;
			default:
				tempRes = true;
				break;
		}
		switch (item.filterOperator)
		{
			case AND: res &= tempRes; break;
			case OR: res |= tempRes; break;
		}
	}
	return res;
}


// -----------------------------------------------------------------------------------------------
// PasportBaseViewWidget
// -----------------------------------------------------------------------------------------------

// Конструктор для таблицы паспортов заказов
PasportBaseViewWidget::PasportBaseViewWidget(int pre, QWidget *parent) :
    QWidget(parent),
	ui(new Ui::PasportBaseViewWidget),
	preOrNew(pre)
{
	ui->setupUi(this);

	ui->copyButton->hide();
	ui->editButton->hide();

	// фильтр таблицы паспортов-заказов
	pasportTableFilter = new FilterTable(this);
	QObject::connect(pasportTableFilter, &FilterTable::filterChanged,
					 this, &PasportBaseViewWidget::pasportTableFilterChanged);
	pasportTableFilter->addLineEdit();
	ui->horizontalLayout_5->addWidget(pasportTableFilter);

    // таблица паспортов-заказов на офсет
	pasportOfsetSourseModel = new QSqlTableModel(this, db);
	pasportOfsetSourseModel->setTable("pasport_ofset");
	pasportOfsetSourseModel->setEditStrategy(QSqlTableModel::EditStrategy::OnFieldChange);
	pasportOfsetSourseModel->select();

	pasportOfsetModel = new TPasportModel(pre, this, db);
	pasportOfsetModel->setDynamicSortFilter(false);
	pasportOfsetModel->setSourceModel(pasportOfsetSourseModel);

    pasportOfsetModel->setHeaderData(0, Qt::Horizontal, QObject::tr("№"));
    pasportOfsetModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Выдать"));
    pasportOfsetModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Наименование продукции"));
    pasportOfsetModel->setHeaderData(15, Qt::Horizontal, QObject::tr("Тираж"));
    pasportOfsetModel->setHeaderData(20, Qt::Horizontal, QObject::tr("Заказчик"));
    pasportOfsetModel->setHeaderData(21, Qt::Horizontal, QObject::tr("Вид"));
    pasportOfsetModel->setHeaderData(22, Qt::Horizontal, QObject::tr("Создан"));
	pasportOfsetModel->setHeaderData(24, Qt::Horizontal, QObject::tr("Печать"));
	pasportOfsetModel->setHeaderData(tPasportOfset::paid, Qt::Horizontal, QObject::tr("Опл"));
	pasportOfsetModel->setHeaderData(tPasportOfset::printed, Qt::Horizontal, QObject::tr("Отп"));
	pasportOfsetModel->setHeaderData(tPasportOfset::prepared, Qt::Horizontal, QObject::tr("Гот"));
	pasportOfsetModel->setHeaderData(tPasportOfset::issued, Qt::Horizontal, QObject::tr("Выд"));

	ui->pasportOfsetTableView->setModel(pasportOfsetModel);
	ui->pasportOfsetTableView->sortByColumn(0, Qt::DescendingOrder);
    ui->pasportOfsetTableView->hideColumn(1);
    ui->pasportOfsetTableView->hideColumn(4);
    ui->pasportOfsetTableView->hideColumn(5);
    ui->pasportOfsetTableView->hideColumn(6);
    ui->pasportOfsetTableView->hideColumn(7);
    ui->pasportOfsetTableView->hideColumn(8);
    ui->pasportOfsetTableView->hideColumn(9);
    ui->pasportOfsetTableView->hideColumn(10);
    ui->pasportOfsetTableView->hideColumn(11);
    ui->pasportOfsetTableView->hideColumn(12);
    ui->pasportOfsetTableView->hideColumn(13);
    ui->pasportOfsetTableView->hideColumn(14);
    ui->pasportOfsetTableView->hideColumn(16);
    ui->pasportOfsetTableView->hideColumn(17);
    ui->pasportOfsetTableView->hideColumn(18);
    ui->pasportOfsetTableView->hideColumn(19);
	ui->pasportOfsetTableView->hideColumn(23);
	if (pre)
	{
		ui->pasportOfsetTableView->hideColumn(tPasportOfset::printDate);
		ui->pasportOfsetTableView->hideColumn(tPasportOfset::issueDate);
		ui->pasportOfsetTableView->hideColumn(tPasportOfset::paid);
		ui->pasportOfsetTableView->hideColumn(tPasportOfset::printed);
		ui->pasportOfsetTableView->hideColumn(tPasportOfset::prepared);
		ui->pasportOfsetTableView->hideColumn(tPasportOfset::issued);

	}

    ui->pasportOfsetTableView->horizontalHeader()->swapSections(22, 1);
    ui->pasportOfsetTableView->horizontalHeader()->swapSections(21, 3);
    ui->pasportOfsetTableView->horizontalHeader()->swapSections(20, 4);
    ui->pasportOfsetTableView->horizontalHeader()->swapSections(21, 5);
    ui->pasportOfsetTableView->horizontalHeader()->swapSections(15, 6);
	ui->pasportOfsetTableView->horizontalHeader()->moveSection(24, 2);
	ui->pasportOfsetTableView->horizontalHeader()->moveSection(tPasportOfset::paid, 4);
	ui->pasportOfsetTableView->horizontalHeader()->moveSection(tPasportOfset::printed, 5);
	ui->pasportOfsetTableView->horizontalHeader()->moveSection(tPasportOfset::prepared, 6);
	ui->pasportOfsetTableView->horizontalHeader()->moveSection(tPasportOfset::issued, 7);



	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(2, QHeaderView::ResizeToContents);
	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(3, QHeaderView::Stretch);
	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(15, QHeaderView::ResizeToContents);
	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(20, QHeaderView::Stretch);
	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(22, QHeaderView::ResizeToContents);
	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(21, QHeaderView::ResizeToContents);
	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(24, QHeaderView::ResizeToContents);
	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(tPasportOfset::paid, QHeaderView::ResizeToContents);
	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(tPasportOfset::printed, QHeaderView::ResizeToContents);
	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(tPasportOfset::prepared, QHeaderView::ResizeToContents);
	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(tPasportOfset::issued, QHeaderView::ResizeToContents);



    // создаем контекстное меню для таблицы паспортов-заказов
	pasportTableViewContMenu = new QMenu(ui->pasportOfsetTableView);
	newSubMenu = new QMenu(pasportTableViewContMenu);
	createPasportTableViewContMenu();

	ui->deletedGroupBox->setChecked(true);

	ui->pasportOfsetTableView->selectRow(0);
}

// Конструктор для таблицы распределения заказов (конкретная дата)
PasportBaseViewWidget::PasportBaseViewWidget(QDate date, int pre, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PasportBaseViewWidget),
	preOrNew(pre)
{
	ui->setupUi(this);

	ui->copyButton->hide();
	ui->editButton->hide();
	ui->deletedGroupBox->hide();
	ui->verticalSpacer->changeSize(0, 0);

	// фильтр таблицы паспортов-заказов
//	pasportTableFilter = new FilterTable(this);
//	QObject::connect(pasportTableFilter, &FilterTable::filterChanged,
//					 this, &PasportBaseViewWidget::pasportTableFilterChanged);
//	pasportTableFilter->addLineEdit();
////	pasportTableFilter->addCheckBox("Скрыть удаленные", "deleted is not true");
//	ui->horizontalLayout_5->addWidget(pasportTableFilter);

	// таблица паспортов-заказов на офсет
	pasportOfsetSourseModel = new QSqlTableModel(this, db);
	pasportOfsetSourseModel->setTable("pasport_ofset");
	pasportOfsetSourseModel->setEditStrategy(QSqlTableModel::EditStrategy::OnFieldChange);
	pasportOfsetSourseModel->setFilter("print_date = '" + QVariant(date).toString() + "'");
	pasportOfsetSourseModel->select();

	pasportOfsetModel = new TPasportModel(pre, this, db);
	pasportOfsetModel->setDynamicSortFilter(false);
	pasportOfsetModel->setSourceModel(pasportOfsetSourseModel);

	pasportOfsetModel->setHeaderData(0, Qt::Horizontal, QObject::tr("№"));
	pasportOfsetModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Выдать"));
	pasportOfsetModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Наименование продукции"));
	pasportOfsetModel->setHeaderData(15, Qt::Horizontal, QObject::tr("Тираж"));
	pasportOfsetModel->setHeaderData(20, Qt::Horizontal, QObject::tr("Заказчик"));
	pasportOfsetModel->setHeaderData(21, Qt::Horizontal, QObject::tr("Вид"));
	pasportOfsetModel->setHeaderData(22, Qt::Horizontal, QObject::tr("Создан"));
	pasportOfsetModel->setHeaderData(24, Qt::Horizontal, QObject::tr("Печать"));

	ui->pasportOfsetTableView->setModel(pasportOfsetModel);
	ui->pasportOfsetTableView->sortByColumn(0, Qt::DescendingOrder);
	ui->pasportOfsetTableView->hideColumn(1);
	ui->pasportOfsetTableView->hideColumn(4);
	ui->pasportOfsetTableView->hideColumn(5);
	ui->pasportOfsetTableView->hideColumn(6);
	ui->pasportOfsetTableView->hideColumn(7);
	ui->pasportOfsetTableView->hideColumn(8);
	ui->pasportOfsetTableView->hideColumn(9);
	ui->pasportOfsetTableView->hideColumn(10);
	ui->pasportOfsetTableView->hideColumn(11);
	ui->pasportOfsetTableView->hideColumn(12);
	ui->pasportOfsetTableView->hideColumn(13);
	ui->pasportOfsetTableView->hideColumn(14);
	ui->pasportOfsetTableView->hideColumn(16);
	ui->pasportOfsetTableView->hideColumn(17);
	ui->pasportOfsetTableView->hideColumn(18);
	ui->pasportOfsetTableView->hideColumn(19);
	ui->pasportOfsetTableView->hideColumn(23);
	ui->pasportOfsetTableView->hideColumn(tPasportOfset::creationDate);
	ui->pasportOfsetTableView->hideColumn(tPasportOfset::issueDate);
	ui->pasportOfsetTableView->hideColumn(tPasportOfset::printDate);
	ui->pasportOfsetTableView->hideColumn(tPasportOfset::paid);
	ui->pasportOfsetTableView->hideColumn(tPasportOfset::printed);
	ui->pasportOfsetTableView->hideColumn(tPasportOfset::prepared);
	ui->pasportOfsetTableView->hideColumn(tPasportOfset::issued);
	ui->pasportOfsetTableView->hideColumn(tPasportOfset::paperPrice);

	if (pre)
	{
		ui->pasportOfsetTableView->hideColumn(tPasportOfset::printDate);
		ui->pasportOfsetTableView->hideColumn(tPasportOfset::issueDate);
	}

	ui->pasportOfsetTableView->horizontalHeader()->swapSections(22, 1);
	ui->pasportOfsetTableView->horizontalHeader()->swapSections(21, 3);
	ui->pasportOfsetTableView->horizontalHeader()->swapSections(20, 4);
	ui->pasportOfsetTableView->horizontalHeader()->swapSections(21, 5);
	ui->pasportOfsetTableView->horizontalHeader()->swapSections(15, 6);
	ui->pasportOfsetTableView->horizontalHeader()->moveSection(24, 2);

	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(2, QHeaderView::ResizeToContents);
	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(3, QHeaderView::Stretch);
	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(15, QHeaderView::ResizeToContents);
	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(20, QHeaderView::Stretch);
	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(22, QHeaderView::ResizeToContents);
	ui->pasportOfsetTableView->horizontalHeader()->
			setSectionResizeMode(21, QHeaderView::ResizeToContents);

	// создаем контекстное меню для таблицы паспортов-заказов
	pasportTableViewContMenu = new QMenu(ui->pasportOfsetTableView);
	newSubMenu = new QMenu(pasportTableViewContMenu);
	createPasportTableViewContMenu();

	pasportTableViewContMenu->removeAction(deleteAction);
	pasportTableViewContMenu->removeAction(copyAction);
	pasportTableViewContMenu->removeAction(recoverAction);
	newSubMenu->removeAction(newHeidelberg);
	newSubMenu->removeAction(newXerox);
	newSubMenu->setEnabled(false);

	ui->deletedGroupBox->setChecked(true);

	ui->pasportOfsetTableView->selectRow(0);
}

PasportBaseViewWidget::~PasportBaseViewWidget()
{
    delete ui;
    delete pasportOfsetModel;
}

// Контекстное меню для таблицы паспортов заказов
void PasportBaseViewWidget::createPasportTableViewContMenu()
{
	preToNewAction = pasportTableViewContMenu->addAction("Создать паспорт-заказ на основе расчета");
	connect(preToNewAction, SIGNAL(triggered()),
			this, SLOT(preToNew()));

	pasportTableViewContMenu->addSeparator();

	newSubMenu->setTitle("Создать");
	pasportTableViewContMenu->addMenu(newSubMenu);

	/*QAction **/newHeidelberg = newSubMenu->addAction("Heidelberg");
	connect(newHeidelberg, SIGNAL(triggered()),
			parent(), SLOT(on_newHeidelberg_triggered()));

	/*QAction **/newXerox = newSubMenu->addAction("Xerox");
	connect(newXerox, SIGNAL(triggered()),
			parent(), SLOT(on_newXerox_triggered()));

	/*QAction **/editAction = pasportTableViewContMenu->addAction("Редактировать");
	connect(editAction, SIGNAL(triggered()),
			this, SLOT(on_editButton_clicked()));

	/*QAction **/copyAction = pasportTableViewContMenu->addAction("Скопировать");
	connect(copyAction, SIGNAL(triggered()),
			this, SLOT(on_copyButton_clicked()));

	pasportTableViewContMenu->addSeparator();

	/*QAction **/printAction = pasportTableViewContMenu->addAction("Печать...");
	connect(printAction, SIGNAL(triggered()),
			this, SLOT(printReport()));

	pasportTableViewContMenu->addSeparator();

	paidAction = pasportTableViewContMenu->addAction("Оплачен");
	paidAction->setCheckable(true);
	connect(paidAction, SIGNAL(triggered()),
			this, SLOT(onPaidClicked()));

	printedAction = pasportTableViewContMenu->addAction("Отпечатан");
	printedAction->setCheckable(true);
	connect(printedAction, SIGNAL(triggered()),
			this, SLOT(onPrintedClicked()));

	preparedAction = pasportTableViewContMenu->addAction("Готов");
	preparedAction->setCheckable(true);
	connect(preparedAction, SIGNAL(triggered()),
			this, SLOT(onPreperedClicked()));

	issuedAction = pasportTableViewContMenu->addAction("Выдан");
	issuedAction->setCheckable(true);
	connect(issuedAction, SIGNAL(triggered()),
			this, SLOT(onIssuedClicked()));

	pasportTableViewContMenu->addSeparator();

	/*QAction **/deleteAction = pasportTableViewContMenu->addAction("Удалить");
	connect(deleteAction, SIGNAL(triggered()),
			this, SLOT(onDeleteButtonCliked()));

	recoverAction = pasportTableViewContMenu->addAction("Восстановить");
	connect(recoverAction, SIGNAL(triggered()),
			this, SLOT(recoverPasport()));
}

void PasportBaseViewWidget::refreshTable()
{
	pasportOfsetSourseModel->select();
	QModelIndex sourceIndex;
	if (!preOrNew)
		sourceIndex = pasportOfsetSourseModel->index(maxId(pasportOfsetSourseModel), 0);
	else sourceIndex = pasportOfsetSourseModel->index(minId(pasportOfsetSourseModel), 0);
	QModelIndex proxyIndex = pasportOfsetModel->mapFromSource(sourceIndex);
	ui->pasportOfsetTableView->selectRow(proxyIndex.row());
	ui->pasportOfsetTableView->setFocus();
}

void PasportBaseViewWidget::refreshCurrentDateTable(long long row)
{
	pasportOfsetSourseModel->select();
	ui->pasportOfsetTableView->selectRow(row);
	ui->pasportOfsetTableView->setFocus();
}

QModelIndex PasportBaseViewWidget::curIndex()
{
	return ui->pasportOfsetTableView->currentIndex();
}

void PasportBaseViewWidget::refreshTableAfterEdit(long long id)
{
	pasportOfsetSourseModel->select();

	int i;
	for (i = 0; i < pasportOfsetModel->rowCount(); ++i)
	{
		QModelIndex idx = pasportOfsetModel->index(i, tPasportOfset::id);
		long long data = pasportOfsetModel->data(idx).toLongLong();
		if (preOrNew) data = -data;
		if (data == id)
			break;
	}

    ui->pasportOfsetTableView->selectRow(i);
	ui->pasportOfsetTableView->setFocus();
}

void PasportBaseViewWidget::on_refreshPushButton_clicked()
{
	int row = ui->pasportOfsetTableView->currentIndex().row();
	int id = pasportOfsetModel->data(pasportOfsetModel->index(row, 0)).toInt();
	if (preOrNew) id = -id;
	pasportOfsetSourseModel->select();
	QModelIndex sourceIndex = pasportOfsetSourseModel->
							  index(curRow(id, pasportOfsetSourseModel), 0);
	QModelIndex proxyIndex = pasportOfsetModel->mapFromSource(sourceIndex);


	ui->pasportOfsetTableView->selectRow(proxyIndex.row());
	ui->pasportOfsetTableView->setFocus();
}

void PasportBaseViewWidget::on_pasportOfsetTableView_doubleClicked(const QModelIndex &index)
{
	if (!index.isValid())
		return;

	QSqlRecord record;
	record = pasportOfsetSourseModel->record(pasportOfsetModel->mapToSource(index).row());

	if (record.value(tPasportOfset::deleted).toBool())
	{
		QMessageBox msg;
		msg.setText("Удаленный заказ нельзя редактировать.\n "
					"Вы можете создать на его основе новый заказ, "
					"выбрав в контекстном меню пункт \"Скопировать\".");
		msg.setIcon(QMessageBox::Warning);
		msg.exec();
		return;
	}

	if (record.value(tPasportOfset::printed).toBool())
	{
		QMessageBox msg;
		msg.setText("Тираж уже отпечатан, паспорт-заказ нельзя изменить.");
		msg.setIcon(QMessageBox::Warning);
		msg.exec();
		return;
	}
	workType = record.value(tPasportOfset::workType).toString();

	zOfsWin = new ZOfsetDialog(record, workType, preOrNew, EditOrCopy::edit);

	QObject::connect(this, &PasportBaseViewWidget::closeAll,
					 zOfsWin, &ZOfsetDialog::close);

	QObject::connect(zOfsWin, &ZOfsetDialog::refreshPasportTableAfterEdit,
					 this, &PasportBaseViewWidget::refreshTableAfterEdit);

	// при сохранении изменений в паспорте-заказе передаем сигнал
	// для обновления обеих таблиц заказов (основной и распределения)
	// сигнал вызывается в функциях
	//		void PasportBaseViewWidget::onDeleteButtonCliked()
	//		void ZOfsetDialog::on_reSaveButton_clicked()
	QObject::connect(zOfsWin, &ZOfsetDialog::refreshAll,
					 this, &PasportBaseViewWidget::refreshAll);

	zOfsWin->setAttribute(Qt::WA_DeleteOnClose);
	zOfsWin->show();
}

void PasportBaseViewWidget::on_editButton_clicked()
{
	QModelIndex index = ui->pasportOfsetTableView->currentIndex();
	on_pasportOfsetTableView_doubleClicked(index);
}

void PasportBaseViewWidget::on_copyButton_clicked()
{
	QModelIndex index = ui->pasportOfsetTableView->currentIndex();
	if (!index.isValid())
		return;
	QSqlRecord record;
	record = pasportOfsetSourseModel->record(pasportOfsetModel->mapToSource(index).row());
	workType = record.value(tPasportOfset::workType).toString();

	if (!db.isOpen())
	{
		QMessageBox msg;
		msg.setText("Не удалось подключиться к базе данных!\n"
					+ db.lastError().text());
		msg.exec();
		return;
	}

	zOfsWin = new ZOfsetDialog(record, workType, preOrNew, EditOrCopy::copy);

	QObject::connect(this, &PasportBaseViewWidget::closeAll,
					 zOfsWin, &ZOfsetDialog::close);

	QObject::connect(zOfsWin, &ZOfsetDialog::refreshPasportTable,
					 this, &PasportBaseViewWidget::refreshTable);

	zOfsWin->setAttribute(Qt::WA_DeleteOnClose);
	zOfsWin->show();
}

void PasportBaseViewWidget::onPaidClicked()
{
	QModelIndex index = ui->pasportOfsetTableView->currentIndex();
	if (!index.isValid())
		return;
	long long row = index.row();
	bool isPaid = paidAction->isChecked();
	if (isPaid)
	{
		isPaid = true;
	}
	else
	{
		isPaid = false;
	}
	pasportOfsetSourseModel->setData(pasportOfsetModel->
									 mapToSource(pasportOfsetModel->index(row, tPasportOfset::paid)),
									 isPaid);
	emit refreshAll();
}

void PasportBaseViewWidget::onPrintedClicked()
{
	QModelIndex index = ui->pasportOfsetTableView->currentIndex();
	if (!index.isValid())
		return;
	long long row = index.row();
	bool isPrinted = printedAction->isChecked();
	if (isPrinted)
	{
		isPrinted = true;
	}
	else
	{
		isPrinted = false;
	}
	pasportOfsetSourseModel->setData(pasportOfsetModel->
									 mapToSource(pasportOfsetModel->index(row, tPasportOfset::printed)),
									 isPrinted);
	emit refreshAll();
}

void PasportBaseViewWidget::onPreperedClicked()
{
	QModelIndex index = ui->pasportOfsetTableView->currentIndex();
	if (!index.isValid())
		return;
	long long row = index.row();
	bool isPrerared = preparedAction->isChecked();
	if (isPrerared)
	{
		isPrerared = true;
	}
	else
	{
		isPrerared = false;
	}
	pasportOfsetSourseModel->setData(pasportOfsetModel->
									 mapToSource(pasportOfsetModel->index(row, tPasportOfset::prepared)),
									 isPrerared);
	emit refreshAll();
}

void PasportBaseViewWidget::onIssuedClicked()
{
	QModelIndex index = ui->pasportOfsetTableView->currentIndex();
	if (!index.isValid())
		return;
	long long row = index.row();
	bool isIssued = issuedAction->isChecked();
	if (isIssued)
	{
		isIssued = true;
	}
	else
	{
		isIssued = false;
	}
	pasportOfsetSourseModel->setData(pasportOfsetModel->
									 mapToSource(pasportOfsetModel->index(row, tPasportOfset::issued)),
									 isIssued);
	emit refreshAll();
}



void PasportBaseViewWidget::onDeleteButtonCliked()
{
    QMessageBox msg;
    msg.setText("Вы уверены, что хотите удалить паспорт-заказ? \n "
				"Удаленные заказы можно восстановить, выбрав \n"
				"в контекстном меню пункт \"Восстановить\".");
    msg.setWindowTitle("Удаление");
    msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msg.setIcon(QMessageBox::Warning);
    msg.setDefaultButton(QMessageBox::Cancel);
    int chois = msg.exec();
    if (chois == QMessageBox::Cancel)
    {
        ui->pasportOfsetTableView->setFocus();
        return;
    }

    int row = ui->pasportOfsetTableView->currentIndex().row();
	QModelIndex sourceIdx = pasportOfsetModel->
					mapToSource(pasportOfsetModel->index(row, tPasportOfset::deleted));

	pasportOfsetSourseModel->setData(sourceIdx, true);
	sourceIdx = pasportOfsetModel->
				mapToSource(pasportOfsetModel->index(row, tPasportOfset::printDate));
	pasportOfsetSourseModel->setData(sourceIdx, QVariant(0).toDate());

    if(row >= pasportOfsetModel->rowCount())
        --row;
	ui->pasportOfsetTableView->selectRow(row);

	emit refreshAll();
}

void PasportBaseViewWidget::recoverPasport()
{
	int row = ui->pasportOfsetTableView->currentIndex().row();
	QModelIndex sourceIdx = pasportOfsetModel->
					mapToSource(pasportOfsetModel->index(row, tPasportOfset::deleted));

	pasportOfsetSourseModel->setData(sourceIdx, false);
	on_refreshPushButton_clicked();

	if(row >= pasportOfsetModel->rowCount())
		--row;
	ui->pasportOfsetTableView->selectRow(row);
}

void PasportBaseViewWidget::preToNew()
{
	QModelIndex idx = ui->pasportOfsetTableView->currentIndex();
	idx = pasportOfsetModel->index(idx.row(), tPasportOfset::id);
	idx = pasportOfsetModel->mapToSource(idx);
	PasportOfset temp;
	pasportOfsetSourseModel->setData(idx, temp.newPasportId(db, "pasport_ofset"));
	on_refreshPushButton_clicked();
}

void PasportBaseViewWidget::on_pasportOfsetTableView_customContextMenuRequested(const QPoint &pos)
{
	if(!ui->pasportOfsetTableView->indexAt(pos).isValid()) return;

	QModelIndex idx = ui->pasportOfsetTableView->indexAt(pos);
	QSqlRecord record = pasportOfsetSourseModel->record(pasportOfsetModel->mapToSource(idx).row());

	if (record.value(tPasportOfset::id).toLongLong() < 0)
	{
		preToNewAction->setEnabled(true);
		preToNewAction->setVisible(true);
	}
	else
	{
		preToNewAction->setDisabled(true);
		preToNewAction->setVisible(false);
	}

	// оплачен
	if (record.value(tPasportOfset::paid).toBool())
	{
		paidAction->setChecked(true);
	}
	else
	{
		paidAction->setChecked(false);
	}

	// отпечатан?
	if (record.value(tPasportOfset::printed).toBool())
	{
		printedAction->setChecked(true);
	}
	else
	{
		printedAction->setChecked(false);
	}

	// готов?
	if (record.value(tPasportOfset::prepared).toBool())
	{
		preparedAction->setChecked(true);
	}
	else
	{
		preparedAction->setChecked(false);
	}

	// выдан?
	if (record.value(tPasportOfset::issued).toBool())
	{
		issuedAction->setChecked(true);
	}
	else
	{
		issuedAction->setChecked(false);
	}

	// устанавливаем активность этапов работы

	// не удален?
	if (!record.value(tPasportOfset::deleted).toBool())
	{
		// не предварительный расчет?
		if (record.value(tPasportOfset::id).toLongLong() >= 0)
		{
			printAction->setEnabled(true);
			paidAction->setEnabled(true);
			if (record.value(tPasportOfset::issued).toBool())
			{
				printedAction->setEnabled(false);
				preparedAction->setEnabled(false);
				issuedAction->setEnabled(true);
			}
			else if (record.value(tPasportOfset::prepared).toBool())
			{
				printedAction->setEnabled(false);
				preparedAction->setEnabled(true);
				issuedAction->setEnabled(true);
			}
			else if (record.value(tPasportOfset::printed).toBool())
			{
				printedAction->setEnabled(true);
				preparedAction->setEnabled(true);
				issuedAction->setEnabled(false);
			}
			else
			{
				printedAction->setEnabled(true);
				preparedAction->setEnabled(false);
				issuedAction->setEnabled(false);
			}
		}
		else
		// предварительный расчет
		{
			printAction->setEnabled(false);
			paidAction->setEnabled(false);
			printedAction->setEnabled(false);
			preparedAction->setEnabled(false);
			issuedAction->setEnabled(false);
		}
		editAction->setEnabled(true);
		deleteAction->setEnabled(true);
		recoverAction->setDisabled(true);
	}
	else
	// удален
	{
		paidAction->setEnabled(false);
		printedAction->setEnabled(false);
		preparedAction->setEnabled(false);
		issuedAction->setEnabled(false);

		editAction->setDisabled(true);
		deleteAction->setDisabled(true);
		recoverAction->setEnabled(true);
	}

    if(pasportTableViewContMenu)
        pasportTableViewContMenu->exec(ui->pasportOfsetTableView->mapToGlobal(pos));
}

void PasportBaseViewWidget::pasportTableFilterChanged(QString filter)
{
	pasportOfsetSourseModel->setFilter(filter);
}

void PasportBaseViewWidget::on_heidelbergCheckBox_toggled(bool checked)
{
	TPasportModel::FilterItem item = {tPasportOfset::workType,
									  settings::heidelberhWorkType,
									  TPasportModel::NotEqual,
									  TPasportModel::AND};

	if(!checked)
		pasportOfsetModel->addFilter(item);
	else pasportOfsetModel->eraseFilter(item);
	ui->pasportOfsetTableView->setFocus();
}

void PasportBaseViewWidget::on_xeroxCheckBox_toggled(bool checked)
{
	TPasportModel::FilterItem item = {tPasportOfset::workType,
									  settings::xeroxWorkType,
									  TPasportModel::NotEqual,
									  TPasportModel::AND};

	if(!checked)
		pasportOfsetModel->addFilter(item);
	else pasportOfsetModel->eraseFilter(item);
	ui->pasportOfsetTableView->setFocus();
}


void PasportBaseViewWidget::on_deletedHideRadioButton_toggled(bool checked)
{
	TPasportModel::FilterItem item = {tPasportOfset::deleted,
									  "true",
									  TPasportModel::NotEqual,
									  TPasportModel::AND};

	if(checked)
		pasportOfsetModel->addFilter(item);
	else pasportOfsetModel->eraseFilter(item);
	ui->pasportOfsetTableView->setFocus();
}


void PasportBaseViewWidget::on_deletedOnlyRadioButton_toggled(bool checked)
{
	TPasportModel::FilterItem item = {tPasportOfset::deleted,
									  "true",
									  TPasportModel::Equal,
									  TPasportModel::AND};

	if(checked)
		pasportOfsetModel->addFilter(item);
	else pasportOfsetModel->eraseFilter(item);
	ui->pasportOfsetTableView->setFocus();
}


void PasportBaseViewWidget::on_deletedGroupBox_toggled(bool arg1)
{
	if (arg1)
	{
		ui->deletedHideRadioButton->isChecked() ?
		on_deletedHideRadioButton_toggled(ui->deletedHideRadioButton->isChecked()) :
		on_deletedOnlyRadioButton_toggled(ui->deletedOnlyRadioButton->isChecked());
	}
	else
	{
		TPasportModel::FilterItem item = {tPasportOfset::deleted,
										  "true",
										  TPasportModel::Equal,
										  TPasportModel::AND};
		pasportOfsetModel->eraseFilter(item);
	}
	ui->pasportOfsetTableView->setFocus();
}

void PasportBaseViewWidget::printReport()
{
//	QModelIndex idx = ui->pasportOfsetTableView->currentIndex();
//	QSqlRecord record = pasportOfsetSourseModel->record(pasportOfsetModel->mapToSource(idx).row());

//	PrintReport report(record);
//	report.print();
////	QFile report(":/reports/reportOfset.html");
////	if (!report.open(QIODevice::ReadOnly | QIODevice::Text))
////	{
////		qDebug() << "Файл не открывается!";
////		return;
////	}
////	qDebug() << "Файл открыт!";
////	QByteArray reportBytes = report.readAll();
////	report.close();
////	QString reportLines = QString(reportBytes);
////	fillReportForm(reportLines);

////	QTextDocument* printReport = new QTextDocument(this);
////	printReport->setHtml(reportLines);
////	printReport->setMetaInformation(QTextDocument::MetaInformation::DocumentTitle, reportLines);
////	printReport->setDocumentMargin(0);
////	printReport->setUseDesignMetrics(true);
////	printReport->setTextWidth(-1);
////	printReport->setUseDesignMetrics(true);

////	QPrinter printer(QPrinter::ScreenResolution);
////	QPrintDialog *dlg = new QPrintDialog(&printer, this);
////	if(!dlg->exec())
////		return;
////	delete dlg;
//////	printer.setResolution(1200);
////	printer.setPageSize(QPageSize::A4);
////	printer.setOutputFormat(QPrinter::OutputFormat::NativeFormat);
////	printer.setPageMargins(QMarginsF(0, 0, 0, 0), QPageLayout::Millimeter);
////	printer.setFullPage(true);

////	printReport->setPageSize(QSizeF(printer.pageRect(QPrinter::Unit::DevicePixel).size()));
////	qDebug() << printReport->pageSize();

//////	QPainter painter(&printer);

//////	printReport->drawContents(&painter);

////	printReport->print(&printer);

////	delete printReport;
}
