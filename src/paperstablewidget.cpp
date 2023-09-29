#include "include/paperstablewidget.h"
#include "ui_paperstablewidget.h"

#include <QMessageBox>

// -----------------------------------------------------------------------------------------------
// TPapersModel
// -----------------------------------------------------------------------------------------------
TPapersModel::TPapersModel(QWidget *parent) :
	TPasportModel(-1, parent)	// -1 чтобы в функции bool TPasportModel::filterAcceptsRow
								// пропускался блок switch (preOrNew)
{

}

TPapersModel::~TPapersModel()
{
}

QVariant TPapersModel::data(const QModelIndex &index, int role) const
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
		if (QSortFilterProxyModel::data(this->index(index.row(), tPapers::deleted)).toBool())
			return QBrush(QColor(128, 128, 128));
	if (role == Qt::BackgroundRole)
		if (QSortFilterProxyModel::data(this->index(index.row(), tPapers::deleted)).toBool())
		{
			if (!(index.row() % 2))
				return QBrush(QColor(255, 235, 235));
			else return QBrush(QColor(255, 225, 225));
		}

	if (role == Qt::DisplayRole)
	{
		return QSortFilterProxyModel::data(index);
	}

	if (role == Qt::EditRole)
		return QSortFilterProxyModel::data(index);

	return QVariant();
}

// -----------------------------------------------------------------------------------------------
// PapersTableWidget
// -----------------------------------------------------------------------------------------------
PapersTableWidget::PapersTableWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PapersTableWidget)
{
	ui->setupUi(this);

	// фильтр таблицы паспортов-заказов
	papersTableFilter = new FilterPapersTable(this);
	QObject::connect(papersTableFilter, &FilterPapersTable::filterChanged,
					 this, &PapersTableWidget::papersTableFilterChanged);
	papersTableFilter->addLineEdit();
	ui->horizontalLayout->addWidget(papersTableFilter);

	papersSourceModel = new QSqlTableModel(this, db);
	papersSourceModel->setTable("papers");
	papersSourceModel->setEditStrategy(QSqlTableModel::EditStrategy::OnFieldChange);
	papersSourceModel->select();

	papersProxyModel = new TPapersModel(this);
	papersProxyModel->setDynamicSortFilter(false);
	papersProxyModel->setSourceModel(papersSourceModel);
	papersProxyModel->sort(tPapers::paperName);

	papersProxyModel->setHeaderData(tPapers::density, Qt::Horizontal, QObject::tr("Плотн."));
	papersProxyModel->setHeaderData(tPapers::paperName, Qt::Horizontal, QObject::tr("Наименование"));
	papersProxyModel->setHeaderData(tPapers::paperType, Qt::Horizontal, QObject::tr("Тип"));
	papersProxyModel->setHeaderData(tPapers::price, Qt::Horizontal, QObject::tr("Цена (руб.)"));
	papersProxyModel->setHeaderData(tPapers::print, Qt::Horizontal, QObject::tr("Печать"));
	papersProxyModel->setHeaderData(tPapers::x, Qt::Horizontal, QObject::tr("Ширина (мм)"));
	papersProxyModel->setHeaderData(tPapers::y, Qt::Horizontal, QObject::tr("Высота (мм)"));

	ui->tableView->setModel(papersProxyModel);

	ui->tableView->horizontalHeader()->moveSection(tPapers::paperName, 0);
	ui->tableView->hideColumn(tPapers::weight);
	ui->tableView->hideColumn(tPapers::coefficient);
	ui->tableView->hideColumn(tPapers::id);
	ui->tableView->hideColumn(tPapers::deleted);

	ui->tableView->horizontalHeader()->
			setSectionResizeMode(tPapers::paperName, QHeaderView::Stretch);
	ui->tableView->horizontalHeader()->
			setSectionResizeMode(tPapers::paperType, QHeaderView::Stretch);
	ui->tableView->horizontalHeader()->
			setSectionResizeMode(tPapers::density, QHeaderView::ResizeToContents);
	ui->tableView->horizontalHeader()->
			setSectionResizeMode(tPapers::print, QHeaderView::ResizeToContents);
	ui->tableView->horizontalHeader()->
			setSectionResizeMode(tPapers::x, QHeaderView::ResizeToContents);
	ui->tableView->horizontalHeader()->
			setSectionResizeMode(tPapers::y, QHeaderView::ResizeToContents);
	ui->tableView->horizontalHeader()->
			setSectionResizeMode(tPapers::price, QHeaderView::ResizeToContents);

	// создаем контекстное меню для таблицы паспортов-заказов
	papersTableContMenu = new QMenu(ui->tableView);
	createPapersTableContMenu();

	ui->deletedGroupBox->setChecked(true);

	ui->tableView->selectRow(0);
	ui->tableView->setFocus();
}

PapersTableWidget::~PapersTableWidget()
{
	delete ui;
	delete papersTableFilter;
	delete papersProxyModel;
	delete papersSourceModel;
}

void PapersTableWidget::refresh(int focusRow)
{
	papersSourceModel->select();
	qDebug() << "row = " << focusRow;
	QModelIndex idx = papersSourceModel->index(focusRow, 0);
	focusRow = papersProxyModel->mapFromSource(idx).row();
	qDebug() << "row = " << focusRow;
	ui->tableView->selectRow(focusRow);
	if (!ui->tableView->currentIndex().isValid())
		ui->tableView->selectRow(0);
	ui->tableView->setFocus();
}

void PapersTableWidget::papersTableFilterChanged(QString filter)
{
	qDebug() << filter;
	papersSourceModel->setFilter(filter);
}

void PapersTableWidget::on_deletedHideRadioButton_toggled(bool checked)
{
	TPasportModel::FilterItem item = {tPapers::deleted,
									  "true",
									  TPasportModel::NotEqual,
									  TPasportModel::AND};

	if(checked)
		papersProxyModel->addFilter(item);
	else papersProxyModel->eraseFilter(item);
	ui->tableView->selectRow(0);
	ui->tableView->setFocus();
}


void PapersTableWidget::on_deletedOnlyRadioButton_toggled(bool checked)
{
	TPasportModel::FilterItem item = {tPapers::deleted,
									  "true",
									  TPasportModel::Equal,
									  TPasportModel::AND};

	if(checked)
		papersProxyModel->addFilter(item);
	else papersProxyModel->eraseFilter(item);
	ui->tableView->selectRow(0);
	ui->tableView->setFocus();
}


void PapersTableWidget::on_deletedGroupBox_toggled(bool arg1)
{
	if (arg1)
	{
		ui->deletedHideRadioButton->isChecked() ?
		on_deletedHideRadioButton_toggled(ui->deletedHideRadioButton->isChecked()) :
		on_deletedOnlyRadioButton_toggled(ui->deletedOnlyRadioButton->isChecked());
	}
	else
	{
		TPasportModel::FilterItem item = {tPapers::deleted,
										  "true",
										  TPasportModel::Equal,
										  TPasportModel::AND};
		papersProxyModel->eraseFilter(item);
	}
	ui->tableView->selectRow(0);
	ui->tableView->setFocus();
}

// -----------------------------------------------------------------------------------------------
// FilterPapersTable
// -----------------------------------------------------------------------------------------------
FilterPapersTable::FilterPapersTable(QWidget *parent) :
	FilterTable(parent)
{

}

FilterPapersTable::~FilterPapersTable()
{

}

void FilterPapersTable::formFilterString()
{
	QString checkBoxString;
	QString lineEditString;
	filter = "";

	// Формируем фильтр из чекбоксов
	if (checkBox.size())
	{
		for (int i = 0; i < checkBox.size(); ++i)
		{
			if (checkBox[i]->isChecked())
			{
				if (!checkBoxString.size())
					checkBoxString += checkBox[i]->filterString;
				else
					checkBoxString += " or " + checkBox[i]->filterString;
			}
		}
		if (checkBoxString.size())
			checkBoxString = "(" + checkBoxString + ")";
	}

	// Формируем фильтр из строки поиска
	QVector<QString> parsWhere = parsingFilterString(lineEdit->text());
	if(parsWhere.size())
	{
		lineEditString = /*"(text(id) LIKE '%" + parsWhere[0] + "%' OR "*/
				"(LOWER(paper_name) LIKE LOWER('%" + parsWhere[0] + "%') OR "
				"LOWER(paper_type) LIKE LOWER('%" + parsWhere[0] + "%') OR "
				"text(x) LIKE LOWER('%" + parsWhere[0] + "%') OR "
				"text(y) LIKE LOWER('%" + parsWhere[0] + "%') OR "
				"text(density) LIKE LOWER('%" + parsWhere[0] + "%'))";
		for(int i = 1; i < parsWhere.size(); ++i)
		{
			lineEditString += /*" and (text(id) LIKE '%" + parsWhere[i] + "%' OR "*/
				" and (LOWER(paper_name) LIKE LOWER('%" + parsWhere[i] + "%') OR "
				"LOWER(paper_type) LIKE LOWER('%" + parsWhere[i] + "%') OR "
				"text(x) LIKE LOWER('%" + parsWhere[i] + "%') OR "
				"text(y) LIKE LOWER('%" + parsWhere[i] + "%') OR "
				"text(density) LIKE LOWER('%" + parsWhere[i] + "%'))";
		}
	}

    // Объединяем фильтры
	if (checkBoxString.size())
		filter = checkBoxString;
	if (!filter.size())
		filter = lineEditString;
	if (checkBoxString.size() && lineEditString.size())
		filter = checkBoxString + " and " + lineEditString;

	emit filterChanged(filter);
}

void PapersTableWidget::on_newPushButton_clicked()
{
	papersTableFilter->clearLineEdit();

	QModelIndex idx = papersProxyModel->index(ui->tableView->currentIndex().row(), tPapers::id);
	int curId = papersProxyModel->data(idx, Qt::DisplayRole).toInt();

	int lastRow = papersSourceModel->rowCount();
	papersSourceModel->insertRow(lastRow);

	newPaper = new NewPaperDialog(papersSourceModel, curId, this);
	newPaper->papersMapper->setCurrentModelIndex(papersSourceModel->index(lastRow, 0)
												/*papersProxyModel->
												 mapToSource(ui->tableView->currentIndex())*/);

	newPaper->newPaperPrep();
	newPaper->show();

	QObject::connect(newPaper, &PaperDialog::refreshPaperTable,
					 this, &PapersTableWidget::refresh);
	ui->tableView->setFocus();
}


void PapersTableWidget::on_tableView_doubleClicked(const QModelIndex &index)
{
	QModelIndex idx = papersProxyModel->index(index.row(), tPapers::id);
	int curId = papersProxyModel->data(idx, Qt::DisplayRole).toInt();
	editPaper = new EditPaperDialog(papersSourceModel, curId, this);
	editPaper->papersMapper->setCurrentModelIndex(papersProxyModel->
												 mapToSource(ui->tableView->currentIndex()));
	editPaper->show();
	QObject::connect(editPaper, &PaperDialog::refreshPaperTable,
					 this, &PapersTableWidget::refresh);
	ui->tableView->setFocus();
}


void PapersTableWidget::on_deletePushButton_clicked()
{
	if (!user.hasDeletePaper())
	{
		QMessageBox msg;
		msg.setText("У вас нет прав для выполнения данной операции.");
		msg.exec();
		return;
	}

	if (papersProxyModel->rowCount() == 0)
		return;

	QMessageBox msg;
	msg.setText("Вы уверены, что хотите удалить выбранные позиции?");
	msg.setWindowTitle("Удаление");
	msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msg.setIcon(QMessageBox::Warning);
	msg.setDefaultButton(QMessageBox::Cancel);
	int chois = msg.exec();
	if (chois == QMessageBox::Cancel)
	{
		ui->tableView->setFocus();
		return;
	}

	QModelIndexList idxs = ui->tableView->selectionModel()->selectedRows();
	for	(QModelIndex i : idxs)
	{
		QModelIndex idx;
		idx = papersProxyModel->index(i.row(), tPapers::deleted);
		idx = papersProxyModel->mapToSource(idx);
		papersSourceModel->setData(idx, QVariant(true));
	}

	int minRow = idxs[0].row();
	for (QModelIndex i : idxs)
	{
		if (i.row() < minRow) minRow = i.row();
		qDebug() << i.row();
	}
	qDebug() << minRow;

	if (minRow >0)
	{
		minRow = papersProxyModel->
				 mapToSource(papersProxyModel->index(minRow-1, tPapers::id)).row();
		refresh(minRow);
	}
	else
	{
		papersSourceModel->select();
		ui->tableView->selectRow(0);
		ui->tableView->setFocus();
	}

}


void PapersTableWidget::on_refreshPushButton_clicked()
{
	QModelIndex idx = papersProxyModel->index(ui->tableView->currentIndex().row(), tPapers::id);
	int curId = papersProxyModel->data(idx, Qt::DisplayRole).toInt();

	papersSourceModel->select();
	int row = curRow(curId, papersSourceModel, tPapers::id);
	idx = papersSourceModel->index(row, 0);
	row = papersProxyModel->mapFromSource(idx).row();
	ui->tableView->selectRow(row);
	ui->tableView->setFocus();
}


void PapersTableWidget::on_editPushButton_clicked()
{
	on_tableView_doubleClicked(ui->tableView->currentIndex());
}


void PapersTableWidget::on_recoverPushButton_clicked()
{
	if (!user.hasDeletePaper())
	{
		QMessageBox msg;
		msg.setText("У вас нет прав для выполнения данной операции.");
		msg.exec();
		return;
	}

	if (papersProxyModel->rowCount() == 0)
		return;
	QModelIndexList idxs = ui->tableView->selectionModel()->selectedRows();
	for	(QModelIndex i : idxs)
	{
		QModelIndex idx;
		idx = papersProxyModel->index(i.row(), tPapers::deleted);
		idx = papersProxyModel->mapToSource(idx);
		papersSourceModel->setData(idx, QVariant(false));
	}

	int minRow = idxs[0].row();
	for (QModelIndex i : idxs)
		if (i.row() < minRow) minRow = i.row();

	if (minRow >0)
	{
		minRow = papersProxyModel->
				 mapToSource(papersProxyModel->index(minRow-1, tPapers::id)).row();
		refresh(minRow);
	}
	else
	{
		papersSourceModel->select();
		ui->tableView->selectRow(0);
		ui->tableView->setFocus();
	}
}

void PapersTableWidget::createPapersTableContMenu()
{
	newPaperCont = papersTableContMenu->addAction("Создать");
	connect(newPaperCont, SIGNAL(triggered()),
			this, SLOT(on_newPushButton_clicked()));

	editPaperCont = papersTableContMenu->addAction("Редактировать");
	connect(editPaperCont, SIGNAL(triggered()),
			this, SLOT(on_editPushButton_clicked()));

	deletePaperCont = papersTableContMenu->addAction("Удалить");
	connect(deletePaperCont, SIGNAL(triggered()),
			this, SLOT(on_deletePushButton_clicked()));

	recoverPaperCont = papersTableContMenu->addAction("Восстановить");
	connect(recoverPaperCont, SIGNAL(triggered()),
			this, SLOT(on_recoverPushButton_clicked()));
}


void PapersTableWidget::on_tableView_customContextMenuRequested(const QPoint &pos)
{
	if (!ui->tableView->indexAt(pos).isValid()) return;

	QModelIndex idx = ui->tableView->indexAt(pos);
	QSqlRecord record = papersSourceModel->record(papersProxyModel->mapToSource(idx).row());

	if (record.value(tPapers::deleted).toBool())
	{
		deletePaperCont->setEnabled(false);
		editPaperCont->setEnabled(false);
		recoverPaperCont->setEnabled(true);

	}
	else
	{
		deletePaperCont->setEnabled(true);
		editPaperCont->setEnabled(true);
		recoverPaperCont->setEnabled(false);
	}

	if (papersTableContMenu)
		papersTableContMenu->exec(ui->tableView->mapToGlobal(pos));
}



void PapersTableWidget::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key::Key_Delete)
		on_deletePushButton_clicked();
	QWidget::keyPressEvent(event);
}
