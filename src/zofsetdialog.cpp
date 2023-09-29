#include "include/zofsetdialog.h"
#include "ui_zofsetdialog.h"
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QObject>
#include <QDate>
#include <QWidget>


// Конструктор для создания нового паспорт-заказа
ZOfsetDialog::ZOfsetDialog(QString pm, int pre, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZOfsetDialog),
	zakaz_ofs(new PasportOfset(/*this*/)),
    postpressModel(new PostpressTableModel(zakaz_ofs->postpress, this)),
    postpressSheetModel(new PostpressTableModel(zakaz_ofs->postpressSheet, this)),
	postpressCirculationModel(new PostpressTableModel(zakaz_ofs->postpressCirculation, this)),
	preOrNew(pre)
{
    this->setWindowFlag(Qt::WindowMinimizeButtonHint);
	this->setWindowFlag(Qt::WindowMaximizeButtonHint);

	initStartParameters(zakaz_ofs->getWorkType(pm, this));

    ui->setupUi(this);
	if (pre)
		this->setWindowTitle("Новый предварительный расчёт " +
							 zakaz_ofs->getPrintingMachine().name +
							 " | Заказ ведет " + user.name.printName);
	else
		this->setWindowTitle("Новый паспорт-заказ " +
							 zakaz_ofs->getPrintingMachine().name +
							 " | Заказ ведет " + user.name.printName);
    ui->trimVal->setValue(zakaz_ofs->getTrimVal());
    ui->issueDateEdit->setDate(QDate::currentDate());
    ui->issueDateEdit->setMinimumDate(QDate::currentDate());
    zakaz_ofs->setCreationDate(QDate::currentDate());
	ui->reSaveButton->hide();
	ui->savePreButton->hide();
	ui->calculatePushButton->hide();
	if (pm == settings::xeroxWorkType)
		xeroxUi();

    // формирование выпадающего списка типов бумаги
    paperTypeList = new QSqlTableModel(this, db);
    paperTypeList->setTable("papers");
	paperTypeList->setFilter(zakaz_ofs->getPrintingMachine().type +
							 " and deleted != 'true'");
	paperTypeList->sort(tPapers::paperName, Qt::SortOrder::AscendingOrder);
	paperTypeList->select();
	ui->paperType->setModel(paperTypeList);
    ui->paperType->setModelColumn(tPapers::paperName);
    ui->paperType->setCurrentIndex(-1);
	ui->paperPriceSpinBox->setEnabled(false);
	ui->paperPriceSpinBox->setValue(0);
	zakaz_ofs->setPaperPrice(0);

    // формирование выпадающего списка размеров экземпляра
    printSizeList = new QSqlTableModel(this,db);
    printSizeList->setTable("print_size");
    printSizeList->select();
    printSizeList->sort(tPrintSize::name, Qt::SortOrder::AscendingOrder);
    ui->instanceSizeList->setModel(printSizeList);
    ui->instanceSizeList->setModelColumn(tPrintSize::name);
    ui->instanceSizeList->setCurrentIndex(-1);

    // формирование списка возможных постпечатных работ
    QSqlQuery postpressQuery = QSqlQuery(db);
	postpressQuery.prepare("SELECT id, name, price, grp, type_ "
						   "FROM postpress WHERE type_ = 1 "
						   "ORDER BY name ASC");
	postpressQuery.exec();
    postpressList.setQuery(std::move(postpressQuery));

    QSqlQuery postpressCirculationQuery = QSqlQuery(db);
	postpressCirculationQuery.prepare("SELECT id, name, price, grp, type_ "
						   "FROM postpress WHERE type_ = 2 "
						   "ORDER BY grp ASC, name ASC");
	postpressCirculationQuery.exec();
    postpressCirculationList.setQuery(std::move(postpressCirculationQuery));

    ui->postpressComboBox->setModel(&postpressList);
    ui->postpressComboBox->setModelColumn(tPostpress::name);
    postpressModel->removeRows(0);

    ui->postpressSheetComboBox->setModel(&postpressList);
    ui->postpressSheetComboBox->setModelColumn(tPostpress::name);
    postpressSheetModel->removeRows(0);

    ui->postpressCirculationComboBox->setModel(&postpressCirculationList);
    ui->postpressCirculationComboBox->setModelColumn(tPostpress::name);
    postpressCirculationModel->removeRows(0);

    // формирование списка выбраных постпечатных работ
    ui->postpressTableView->setModel(postpressModel);
    ui->postpressTableView->hideColumn(0);
    ui->postpressTableView->hideColumn(2);
    ui->postpressTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->postpressTableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->postpressTableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);

    ui->postpressSheetTableView->setModel(postpressSheetModel);
    ui->postpressSheetTableView->hideColumn(0);
    ui->postpressSheetTableView->hideColumn(2);
    ui->postpressSheetTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->postpressSheetTableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->postpressSheetTableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);

    ui->postpressCirculationTableView->setModel(postpressCirculationModel);
    ui->postpressCirculationTableView->hideColumn(0);
    ui->postpressCirculationTableView->hideColumn(2);
    ui->postpressCirculationTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->postpressCirculationTableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->postpressCirculationTableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);


    ui->instanceWdth->setValue(0);
    ui->instanceHght->setValue(0);
    ui->clientSelect->setFocus();
}

// Конструктор для редактирования существующего паспорта-заказа
// или создания нового на основе выбранного паспорта-заказа
ZOfsetDialog::ZOfsetDialog(const QSqlRecord zakaz, QString pm, int pre, int copyOrEdt, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZOfsetDialog),
    zakaz_ofs(new PasportOfset()),
    postpressModel(new PostpressTableModel(zakaz_ofs->postpress, this)),
    postpressSheetModel(new PostpressTableModel(zakaz_ofs->postpressSheet, this)),
    postpressCirculationModel(new PostpressTableModel(zakaz_ofs->postpressCirculation, this)),
	record(zakaz),
	copyOrEdit(copyOrEdt),
	preOrNew(pre)
{
	this->setWindowFlag(Qt::WindowMinimizeButtonHint);
	this->setWindowFlag(Qt::WindowMaximizeButtonHint);

	initStartParameters(zakaz_ofs->getWorkType(pm, this));

	ui->setupUi(this);
	ui->reSaveButton->hide();
	ui->savePreButton->hide();
	ui->calculatePushButton->hide();
	if (pm == settings::xeroxWorkType)
		xeroxUi();

    if(copyOrEdit == EditOrCopy::edit)
    {
		ui->savePreButton->hide();

		QSqlQuery query(db);
		query.prepare("SELECT print_name FROM users WHERE full_name = '"
					  + record.value(tPasportOfset::owner).toString() + "'");
		query.exec();
		query.first();
		if (pre)
		{
			long long id = -zakaz.value(0).toLongLong();
			this->setWindowTitle("Предварительный расчёт "
								 + zakaz_ofs->getPrintingMachine().name
								 + " № "
								 + QVariant(id).toString()
								 + " | Заказ ведет " +
								 query.value(0).toString());
		}
		else
			this->setWindowTitle("Паспорт-заказ "
								 + zakaz_ofs->getPrintingMachine().name
								 + " № "
								 + zakaz.value(0).toString()
								 + " | Заказ ведет " +
								 query.value(0).toString());
    }
    else
    {
        ui->reSaveButton->hide();

		if (pre)
			this->setWindowTitle("Предварительный расчёт " +
								 zakaz_ofs->getPrintingMachine().name +
								 " | Заказ ведет " + user.name.printName);
		else
			this->setWindowTitle("Паспорт-заказ " +
								 zakaz_ofs->getPrintingMachine().name +
								 " | Заказ ведет " + user.name.printName);



		zakaz_ofs->setCreationDate(QDate::currentDate());
    }

    ui->trimVal->setValue(zakaz_ofs->getTrimVal());
    ui->issueDateEdit->setDate(QDate::currentDate());
    ui->issueDateEdit->setMinimumDate(QDate::currentDate());

    // формирование выпадающего списка типов бумаги
	qDebug() << 4;
    paperTypeList = new QSqlTableModel(this, db);
    paperTypeList->setTable("papers");
	paperTypeList->setFilter(zakaz_ofs->getPrintingMachine().type +
							 " and deleted != 'true'");
    paperTypeList->select();
	paperTypeList->sort(tPapers::paperName, Qt::SortOrder::AscendingOrder);
	ui->paperType->setModel(paperTypeList);
    ui->paperType->setModelColumn(tPapers::paperName);
    ui->paperType->setCurrentIndex(-1);
	// формирование выпадающего списка размеров экземпляра
    printSizeList = new QSqlTableModel(this,db);
    printSizeList->setTable("print_size");
    printSizeList->select();
    printSizeList->sort(tPrintSize::name, Qt::SortOrder::AscendingOrder);
    ui->instanceSizeList->setModel(printSizeList);
    ui->instanceSizeList->setModelColumn(tPrintSize::name);
    ui->instanceSizeList->setCurrentIndex(-1);

    // формирование списка возможных постпечатных работ
    QSqlQuery postpressQuery = QSqlQuery(db);
    postpressQuery.prepare("SELECT id, name, price, grp, type_ "
                           "FROM postpress WHERE type_ = 1 "
						   "ORDER BY name ASC");
    postpressQuery.exec();
    postpressList.setQuery(std::move(postpressQuery));



    QSqlQuery postpressCirculationQuery = QSqlQuery(db);
    postpressCirculationQuery.prepare("SELECT id, name, price, grp, type_ "
                           "FROM postpress WHERE type_ = 2 "
						   "ORDER BY name ASC");
    postpressCirculationQuery.exec();
    postpressCirculationList.setQuery(std::move(postpressCirculationQuery));

    ui->postpressComboBox->setModel(&postpressList);
    ui->postpressComboBox->setModelColumn(tPostpress::name);
    postpressModel->removeRows(0);

    ui->postpressSheetComboBox->setModel(&postpressList);
    ui->postpressSheetComboBox->setModelColumn(tPostpress::name);
    postpressSheetModel->removeRows(0);

    ui->postpressCirculationComboBox->setModel(&postpressCirculationList);
    ui->postpressCirculationComboBox->setModelColumn(tPostpress::name);
    postpressCirculationModel->removeRows(0);


    ui->clientSelect->setFocus();

    // Заполнение полей заказа
        // Данные о клиенте
    zakaz_ofs->setClientId(zakaz.value(tPasportOfset::client).toInt());
    zakaz_ofs->setClientName(zakaz.value(tPasportOfset::clientName).toString());
    ui->clientName->setText(zakaz_ofs->getClientName());

        // Дата
    if(zakaz.value(tPasportOfset::issueDate).toDate() >= QDate::currentDate())
        ui->issueDateEdit->setDate(zakaz.value(tPasportOfset::issueDate).toDate());
    else ui->issueDateEdit->setDate(QDate::currentDate());

        // Наименование продукции
    ui->productionlineEdit->setText(zakaz.value(tPasportOfset::production).toString());

        // Тип и размеры бумаги
    int i;
    for(i = 0; i < paperTypeList->rowCount(); ++i)
        if (paperTypeList->data(paperTypeList->index(i, tPapers::id)) == zakaz.value(tPasportOfset::paper).toInt())
            break;
    ui->paperType->setCurrentIndex(i);
	if (record.value(tPasportOfset::paperPrice).toBool())
	{
		ui->paperPriceSpinBox->setValue(record.value(tPasportOfset::paperPrice).toDouble());
		zakaz_ofs->setPaperPrice(record.value(tPasportOfset::paperPrice).toDouble());
	}

        // Размеры экземпляра
    ui->instanceWdth->setValue(zakaz.value(tPasportOfset::instWdth).toInt());
    ui->instanceHght->setValue(zakaz.value(tPasportOfset::instHght).toInt());
    ui->trimVal->setValue(zakaz.value(tPasportOfset::trimVal).toInt());

        // Цветность
    ui->cmykFaceSpinBox->setValue(zakaz.value(tPasportOfset::faceCMYK).toInt());
    ui->pantoneFaceSpinBox->setValue(zakaz.value(tPasportOfset::facePantone).toInt());
    ui->ofsLakFaceCheckBox->setChecked(zakaz.value(tPasportOfset::faceOfsLak).toInt());
    ui->cmykBackSpinBox->setValue(zakaz.value(tPasportOfset::backCMYK).toInt());
    ui->pantoneBackSpinBox->setValue(zakaz.value(tPasportOfset::backPantone).toInt());
    ui->ofsLakBackCheckBox->setChecked(zakaz.value(tPasportOfset::backOfsLak).toInt());
    ui->pantoneListLineEdit->setText(zakaz.value(tPasportOfset::pantoneList).toString());

        // Тираж
    ui->instCountSpinBox->setValue(zakaz.value(tPasportOfset::instCount).toLongLong());

        // Дополнительные сведения к заказу
    ui->infoTextEdit->setText(zakaz.value(tPasportOfset::additionInfo).toString());

        // Формирование моделей выбранных постпечатных работ
    postpressParsing(zakaz.value(tPasportOfset::postpress).toString(),
                     postpressModel,
					 postpressList,
					 zakaz_ofs->postpress);
    postpressParsing(zakaz.value(tPasportOfset::postpressSheet).toString(),
                     postpressSheetModel,
					 postpressList,
					 zakaz_ofs->postpressSheet);
    postpressParsing(zakaz.value(tPasportOfset::postpressCirculation).toString(),
                     postpressCirculationModel,
					 postpressCirculationList,
					 zakaz_ofs->postpressCirculation);
	costRefresh();

        // формирование списка выбраных постпечатных работ
    ui->postpressTableView->setModel(postpressModel);
    ui->postpressTableView->hideColumn(0);
    ui->postpressTableView->hideColumn(2);
    ui->postpressTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->postpressTableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->postpressTableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    if(postpressModel->rowCount())
        ui->postpressTableView->selectRow(0);

    ui->postpressSheetTableView->setModel(postpressSheetModel);
    ui->postpressSheetTableView->hideColumn(0);
    ui->postpressSheetTableView->hideColumn(2);
    ui->postpressSheetTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->postpressSheetTableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->postpressSheetTableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    if(postpressSheetModel->rowCount())
        ui->postpressSheetTableView->selectRow(0);

    ui->postpressCirculationTableView->setModel(postpressCirculationModel);
    ui->postpressCirculationTableView->hideColumn(0);
    ui->postpressCirculationTableView->hideColumn(2);
    ui->postpressCirculationTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->postpressCirculationTableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->postpressCirculationTableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    if(postpressCirculationModel->rowCount())
        ui->postpressCirculationTableView->selectRow(0);

	zakaz_ofs->productionPrice = zakaz.value(tPasportOfset::cost).toDouble();
	ui->price->setText(tlib::conformCostString(zakaz_ofs->productionPrice) + " руб.");
}

ZOfsetDialog::~ZOfsetDialog()
{
    delete zakaz_ofs;
    delete ui;
    delete paperTypeList;
    delete printSizeList;
    delete postpressModel;
    delete postpressSheetModel;
	delete postpressCirculationModel;
}

void ZOfsetDialog::xeroxUi()
{
	ui->pantoneFaceLabel->setEnabled(false);
	ui->pantoneFaceSpinBox->setEnabled(false);
	ui->ofsLakFaceCheckBox->setEnabled(false);
	ui->pantoneBackLabel->setEnabled(false);
	ui->pantoneBackSpinBox->setEnabled(false);
	ui->ofsLakBackCheckBox->setEnabled(false);
	ui->pantoneListLineEdit->setEnabled(false);
}

void ZOfsetDialog::initStartParameters(settings::WorkTypes wt)
{
	// Инициализируем параметры печатной машины
	zakaz_ofs->setPrintingMachine(wt);

	// Инициализируем расходные материалы
	zakaz_ofs->setSupplies();

	ZOfsetDialog::tableName = zakaz_ofs->getPrintingMachine().tableName;

	// Инициализируем указатель на нужную функцию расчета стоимости заказа
	if (wt.name == "Heidelberg")
		calculateProductionPrice = &PasportBase::calculateOfsetProductionPrice;
	else if (wt.name == "Xerox")
		calculateProductionPrice = &PasportBase::calculateXeroxProductionPrice;
}


void ZOfsetDialog::on_instanceWdth_valueChanged(int arg1)
{

    int x = ui->instanceWdth->value();
    int y = ui->instanceHght->value();
    zakaz_ofs->setInstSize(x, y);

    // если размер экземпляра меняется вручную, а не подставляется из выпадающего списка
    // то устанасливаем текущий индекс выпадающего списка в -1
    QModelIndex idx = printSizeList->index(ui->instanceSizeList->currentIndex(),
                                           tPrintSize::x,
                                           QModelIndex());
    int width = printSizeList->data(idx).toInt();
    if (ui->instanceWdth->value() != width)
        ui->instanceSizeList->setCurrentIndex(-1);

    ui->instPerSht->setText(QString::number(zakaz_ofs->getInstPerSheet()));
    ui->preTrimSize->setText(QString::number(zakaz_ofs->getPreTrimWidth())
                             + " x "
                             + QString::number(zakaz_ofs->getPreTrimHeight()));
    refreshSheetsCount();
	costRefresh();
}


void ZOfsetDialog::on_instanceHght_valueChanged(int arg1)
{

    int x = ui->instanceWdth->value();
    int y = ui->instanceHght->value();
    zakaz_ofs->setInstSize(x, y);

    // если размер экземпляра меняется вручную, а не подставляется из выпадающего списка
    // то устанасливаем текущий индекс выпадающего списка в -1
    QModelIndex idx = printSizeList->index(ui->instanceSizeList->currentIndex(),
                                           tPrintSize::y,
                                           QModelIndex());
    int height = printSizeList->data(idx).toInt();
    if (y != height)
        ui->instanceSizeList->setCurrentIndex(-1);

    ui->instPerSht->setText(QString::number(zakaz_ofs->getInstPerSheet()));
    ui->preTrimSize->setText(QString::number(zakaz_ofs->getPreTrimWidth())
                             + " x "
                             + QString::number(zakaz_ofs->getPreTrimHeight()));

    refreshSheetsCount();
	costRefresh();
}


void ZOfsetDialog::on_trimVal_valueChanged(int arg1)
{
    int trim = ui->trimVal->value();
    zakaz_ofs->setTrimVal(trim);

    ui->instPerSht->setText(QString::number(zakaz_ofs->getInstPerSheet()));
    ui->preTrimSize->setText(QString::number(zakaz_ofs->getPreTrimWidth())
                             + " x "
                             + QString::number(zakaz_ofs->getPreTrimHeight()));
    refreshSheetsCount();
	costRefresh();
}

void ZOfsetDialog::on_pushButton_clicked()
{
    this->close();
}

void ZOfsetDialog::on_paperType_currentIndexChanged(int index)
{
    if (index < 0)
    {
        zakaz_ofs->setStockId(-1);
		ui->paperPriceSpinBox->setEnabled(false);
		ui->paperPriceSpinBox->setValue(0.0);
		zakaz_ofs->setStockSize(0, 0);
		ui->printAreaVal->setText(QString::number(zakaz_ofs->getPrintAreaWidth())
								  + " x "
								  + QString::number(zakaz_ofs->getPrintAreaHeight()));
		ui->instPerSht->setText(QString::number(zakaz_ofs->getInstPerSheet()));
		refreshSheetsCount();
		qDebug() << zakaz_ofs->getStockId()
				 << zakaz_ofs->getPaperPrice();

        return;
    }

    QModelIndex idx = paperTypeList->index(ui->paperType->currentIndex(), tPapers::x, QModelIndex());
    int x = paperTypeList->data(idx).toInt();
    idx = paperTypeList->index(ui->paperType->currentIndex(), tPapers::y, QModelIndex());
    int y = paperTypeList->data(idx).toInt();

	// ПРОВЕРКА РАЗМЕРА БУМАГИ НА СООТВЕТСТВИЕ МАКСИМАЛЬНО ДОПУСТИМОМУ
	int maxX = zakaz_ofs->getPrintingMachine().maxPaperWidth;
	int maxY = zakaz_ofs->getPrintingMachine().maxPaperHeight;
	if (x > maxX or y > maxY)
		if (x > maxY or y > maxX)
		{
			QMessageBox msg;
			msg.setText("Размер выбранной бумаги превышает допустимый.");
			msg.exec();
			ui->paperType->setCurrentIndex(-1);
			return;
		}

	idx = paperTypeList->index(ui->paperType->currentIndex(), tPapers::id, QModelIndex());
    int id = paperTypeList->data(idx).toInt();
    zakaz_ofs->setStockId(id);

    zakaz_ofs->setStockSize(x, y);
    ui->printAreaVal->setText(QString::number(zakaz_ofs->getPrintAreaWidth())
                              + " x "
                              + QString::number(zakaz_ofs->getPrintAreaHeight()));
    ui->instPerSht->setText(QString::number(zakaz_ofs->getInstPerSheet()));

    idx = paperTypeList->index(ui->paperType->currentIndex(), tPapers::price, QModelIndex());
    double price = paperTypeList->data(idx).toDouble();
    zakaz_ofs->setPaperPrice(price);
	ui->paperPriceSpinBox->setEnabled(true);
	ui->paperPriceSpinBox->setValue(zakaz_ofs->getPaperPrice());

    refreshSheetsCount();
}

void ZOfsetDialog::on_instanceSizeList_currentIndexChanged(int index)
{
    // Если размер экземпляра изменился вручную (индекс списка размеров
    // сброшен на -1 функцией on_instanceWdth_valueChanged или on_instanceHght_valueChanged),
    // то выйти из функции
    if (ui->instanceSizeList->currentIndex() == -1) return;

    // Иначе определяем индексы ячеек X и Y в строке instanceSizeList->currentIndex(),
    // считываем из них значения и вставляем в поля instanceWdth и instanceHght
    QModelIndex idx = printSizeList->index(ui->instanceSizeList->currentIndex(),
                                           tPrintSize::x,
                                           QModelIndex());
    int x = printSizeList->data(idx).toInt();
    idx = printSizeList->index(ui->instanceSizeList->currentIndex(), tPrintSize::y, QModelIndex());
    int y = printSizeList->data(idx).toInt();
    ui->instanceWdth->setValue(x);
    ui->instanceHght->setValue(y);
}

void ZOfsetDialog::on_paperFilter_textEdited(const QString &arg1)
{
    QString where = ui->paperFilter->text();
    QVector<QString> parsWhere = parsingFilterString(where);

    where = zakaz_ofs->getPrintingMachine().type +
			" and deleted != 'true'";
	foreach (QString s, parsWhere)
		where += " and LOWER(paper_name) LIKE LOWER('%" + s + "%')";

    paperTypeList->setFilter(where);
}

void ZOfsetDialog::on_clientSelect_clicked()
{
    selectClient = new ClientSelectDialog(this, zakaz_ofs->getClientId());
    selectClient->setAttribute(Qt::WA_DeleteOnClose);
    selectClient->setWindowFlag(Qt::WindowMinimizeButtonHint);
    selectClient->setWindowModality(Qt::WindowModal);

    selectClient->show();

    QObject::connect(selectClient, &ClientSelectDialog::sendClientId,
                     this, &ZOfsetDialog::setClientId);
}

void ZOfsetDialog::setClientId(int id, QString name)
{
    zakaz_ofs->setClientId(id);
    zakaz_ofs->setClientName(name);
    ui->clientName->setText(name);
}

void ZOfsetDialog::refreshSheetsCount()
{
    ui->sheetsCountLabel->setText(QString::number(zakaz_ofs->getSheetsCount()));
    ui->fittingCountLabel->setText(QString::number(zakaz_ofs->getFittingCount()));
    ui->overallSheetsCountLabel->setText(QString::number(zakaz_ofs->getOverallSheetsCount()));
}

void ZOfsetDialog::on_cmykFaceSpinBox_valueChanged(int arg1)
{
    zakaz_ofs->setCmykFace(arg1);
    zakaz_ofs->setColorCount(zakaz_ofs->getCmykFace(),
                             zakaz_ofs->getPantoneFace(),
                             zakaz_ofs->getOfslakFace(),
                             zakaz_ofs->getCmykBack(),
                             zakaz_ofs->getPantoneBack(),
                             zakaz_ofs->getOfslakBack());
    ui->colorCountLabel->setText(QString::number(zakaz_ofs->getColorFace()) +
                                 " + " +
                                 QString::number(zakaz_ofs->getColorBack()) +
                                 " (" +
                                 QString::number(zakaz_ofs->getRuns()) +
                                 ")");
    refreshSheetsCount();
	costRefresh();
}

void ZOfsetDialog::on_pantoneFaceSpinBox_valueChanged(int arg1)
{
    zakaz_ofs->setPantoneFace(arg1);
    zakaz_ofs->setColorCount(zakaz_ofs->getCmykFace(),
                             zakaz_ofs->getPantoneFace(),
                             zakaz_ofs->getOfslakFace(),
                             zakaz_ofs->getCmykBack(),
                             zakaz_ofs->getPantoneBack(),
                             zakaz_ofs->getOfslakBack());
    ui->colorCountLabel->setText(QString::number(zakaz_ofs->getColorFace()) +
                                 " + " +
                                 QString::number(zakaz_ofs->getColorBack()) +
                                 " (" +
                                 QString::number(zakaz_ofs->getRuns()) +
                                 ")");
    refreshSheetsCount();
	costRefresh();
}

void ZOfsetDialog::on_cmykBackSpinBox_valueChanged(int arg1)
{
    zakaz_ofs->setCmykBack(arg1);
    zakaz_ofs->setColorCount(zakaz_ofs->getCmykFace(),
                             zakaz_ofs->getPantoneFace(),
                             zakaz_ofs->getOfslakFace(),
                             zakaz_ofs->getCmykBack(),
                             zakaz_ofs->getPantoneBack(),
                             zakaz_ofs->getOfslakBack());
    ui->colorCountLabel->setText(QString::number(zakaz_ofs->getColorFace()) +
                                 " + " +
                                 QString::number(zakaz_ofs->getColorBack()) +
                                 " (" +
                                 QString::number(zakaz_ofs->getRuns()) +
                                 ")");
    refreshSheetsCount();
	costRefresh();
}

void ZOfsetDialog::on_pantoneBackSpinBox_valueChanged(int arg1)
{
    zakaz_ofs->setPantoneBack(arg1);
    zakaz_ofs->setColorCount(zakaz_ofs->getCmykFace(),
                             zakaz_ofs->getPantoneFace(),
                             zakaz_ofs->getOfslakFace(),
                             zakaz_ofs->getCmykBack(),
                             zakaz_ofs->getPantoneBack(),
                             zakaz_ofs->getOfslakBack());
    ui->colorCountLabel->setText(QString::number(zakaz_ofs->getColorFace()) +
                                 " + " +
                                 QString::number(zakaz_ofs->getColorBack()) +
                                 " (" +
                                 QString::number(zakaz_ofs->getRuns()) +
                                 ")");
    refreshSheetsCount();
	costRefresh();
}


void ZOfsetDialog::on_ofsLakFaceCheckBox_stateChanged(int arg1)
{
    if (arg1)
        zakaz_ofs->setOfslakFace(1);
    else
        zakaz_ofs->setOfslakFace(0);

    zakaz_ofs->setColorCount(zakaz_ofs->getCmykFace(),
                             zakaz_ofs->getPantoneFace(),
                             zakaz_ofs->getOfslakFace(),
                             zakaz_ofs->getCmykBack(),
                             zakaz_ofs->getPantoneBack(),
                             zakaz_ofs->getOfslakBack());
    ui->colorCountLabel->setText(QString::number(zakaz_ofs->getColorFace()) +
                                 " + " +
                                 QString::number(zakaz_ofs->getColorBack()) +
                                 " (" +
                                 QString::number(zakaz_ofs->getRuns()) +
                                 ")");
    refreshSheetsCount();
	costRefresh();
}


void ZOfsetDialog::on_ofsLakBackCheckBox_stateChanged(int arg1)
{
    if (arg1)
        zakaz_ofs->setOfslakBack(1);
    else
        zakaz_ofs->setOfslakBack(0);

    zakaz_ofs->setColorCount(zakaz_ofs->getCmykFace(),
                             zakaz_ofs->getPantoneFace(),
                             zakaz_ofs->getOfslakFace(),
                             zakaz_ofs->getCmykBack(),
                             zakaz_ofs->getPantoneBack(),
                             zakaz_ofs->getOfslakBack());
    ui->colorCountLabel->setText(QString::number(zakaz_ofs->getColorFace()) +
                                 " + " +
                                 QString::number(zakaz_ofs->getColorBack()) +
                                 " (" +
                                 QString::number(zakaz_ofs->getRuns()) +
                                 ")");
    refreshSheetsCount();
	costRefresh();
}

void ZOfsetDialog::on_instCountSpinBox_valueChanged(int arg1)
{
    zakaz_ofs->setInstCount(arg1);
    refreshSheetsCount();
	costRefresh();
}

void ZOfsetDialog::on_issueDateEdit_userDateChanged(const QDate &date)
{
    zakaz_ofs->setIssueDate(date);
}

void ZOfsetDialog::on_addPostpressButton_clicked()
{
    if (zakaz_ofs->postpress.size() == 0)
        return;

    int i = ui->postpressTableView->currentIndex().row();
    ++zakaz_ofs->postpress[i].count;
    zakaz_ofs->postpress[i].cost = zakaz_ofs->postpress[i].price * zakaz_ofs->postpress[i].count;
    emit postpressModel->dataChanged(postpressModel->index(i, 3), postpressModel->index(i, 4));
    ui->postpressTableView->setFocus();
    ui->postpressTableView->selectRow(i);

	costRefresh();

	return;
}


void ZOfsetDialog::on_removePostpressButton_clicked()
{
    if (!postpressModel->rowCount())
        return;

    int i = ui->postpressTableView->currentIndex().row();
    if (zakaz_ofs->postpress[i].count > 1)
    {
        --zakaz_ofs->postpress[i].count;
        zakaz_ofs->postpress[i].cost = zakaz_ofs->postpress[i].price * zakaz_ofs->postpress[i].count;
        emit postpressModel->dataChanged(postpressModel->index(i, 3), postpressModel->index(i, 4));
        ui->postpressTableView->setFocus();
        ui->postpressTableView->selectRow(i);

		costRefresh();

		return;
    }

    postpressModel->removeRows(i);
    ui->postpressTableView->setFocus();
    if (postpressModel->rowCount())
        ui->postpressTableView->selectRow(i);

	costRefresh();
}

void ZOfsetDialog::on_postpressComboBox_currentIndexChanged(int index)
{
    if (ui->postpressComboBox->currentIndex() == -1)
        return;

    PasportBase::Postpress p;
    QModelIndex idx = postpressList.index(index, tPostpress::price, QModelIndex());
    p.price = postpressList.data(idx).toDouble();
    if (p.price < 0)
    {
        ui->postpressComboBox->setCurrentIndex(-1);
        return;
    }
    idx = postpressList.index(index, tPostpress::id, QModelIndex());
    p.id = postpressList.data(idx).toInt();

    unsigned long long i = 0;
    for (i = 0; i < zakaz_ofs->postpress.size(); ++i)
        if (zakaz_ofs->postpress[i].id == p.id)
            break;

    if (i != zakaz_ofs->postpress.size())
    {
        emit postpressModel->dataChanged(postpressModel->index(i, 3), postpressModel->index(i, 4));
        ui->postpressTableView->setFocus();
        ui->postpressTableView->selectRow(i);
        ui->postpressComboBox->setCurrentIndex(-1);
        return;
    }
    idx = postpressList.index(index, tPostpress::name, QModelIndex());
    p.name = postpressList.data(idx).toString();
    idx = postpressList.index(index, tPostpress::price, QModelIndex());
    p.price = postpressList.data(idx).toDouble();

    postpressModel->insertRows(p.id, p.name, p.price, 1);
    ui->postpressComboBox->setCurrentIndex(-1);
    ui->postpressTableView->setFocus();
    ui->postpressTableView->selectRow(i);

	costRefresh();
}


void ZOfsetDialog::on_postpressSheetComboBox_currentIndexChanged(int index)
{
    if (ui->postpressSheetComboBox->currentIndex() == -1)
        return;

    PasportBase::Postpress p;
    QModelIndex idx = postpressList.index(index, tPostpress::price, QModelIndex());
    p.price = postpressList.data(idx).toDouble();
    if (p.price < 0)
    {
        ui->postpressSheetComboBox->setCurrentIndex(-1);
        return;
    }
    idx = postpressList.index(index, tPostpress::id, QModelIndex());
    p.id = postpressList.data(idx).toInt();

    unsigned long long i = 0;
    for (i = 0; i < zakaz_ofs->postpressSheet.size(); ++i)
        if (zakaz_ofs->postpressSheet[i].id == p.id)
            break;

    if (i != zakaz_ofs->postpressSheet.size())
    {
        emit postpressSheetModel->dataChanged(postpressSheetModel->index(i, 3),
                                              postpressSheetModel->index(i, 4));
        ui->postpressSheetTableView->setFocus();
        ui->postpressSheetTableView->selectRow(i);
        ui->postpressSheetComboBox->setCurrentIndex(-1);
        return;
    }
    idx = postpressList.index(index, tPostpress::name, QModelIndex());
    p.name = postpressList.data(idx).toString();
    idx = postpressList.index(index, tPostpress::price, QModelIndex());
    p.price = postpressList.data(idx).toDouble();

    postpressSheetModel->insertRows(p.id, p.name, p.price, 1);
    ui->postpressSheetComboBox->setCurrentIndex(-1);
    ui->postpressSheetTableView->setFocus();
    ui->postpressSheetTableView->selectRow(i);

	costRefresh();
}


void ZOfsetDialog::on_addPostpressSheetButton_clicked()
{
    if (zakaz_ofs->postpressSheet.size() == 0)
        return;

    int i = ui->postpressSheetTableView->currentIndex().row();
    ++zakaz_ofs->postpressSheet[i].count;
    zakaz_ofs->postpressSheet[i].cost = zakaz_ofs->postpressSheet[i].price * zakaz_ofs->postpressSheet[i].count;
    emit postpressSheetModel->dataChanged(postpressSheetModel->index(i, 3),
                                          postpressSheetModel->index(i, 4));
    ui->postpressSheetTableView->setFocus();
    ui->postpressSheetTableView->selectRow(i);

	costRefresh();

	return;
}


void ZOfsetDialog::on_removePostpressSheetButton_clicked()
{
    if (!postpressSheetModel->rowCount())
        return;

    int i = ui->postpressSheetTableView->currentIndex().row();
    if (zakaz_ofs->postpressSheet[i].count > 1)
    {
        --zakaz_ofs->postpressSheet[i].count;
        zakaz_ofs->postpressSheet[i].cost = zakaz_ofs->postpressSheet[i].price * zakaz_ofs->postpressSheet[i].count;
        emit postpressSheetModel->dataChanged(postpressSheetModel->index(i, 3),
                                              postpressSheetModel->index(i, 4));
        ui->postpressSheetTableView->setFocus();
        ui->postpressSheetTableView->selectRow(i);

		costRefresh();

		return;
    }

    postpressSheetModel->removeRows(i);
    ui->postpressSheetTableView->setFocus();
    if (postpressSheetModel->rowCount())
        ui->postpressSheetTableView->selectRow(i);

	costRefresh();

}


void ZOfsetDialog::on_postpressCirculationComboBox_currentIndexChanged(int index)
{
    if (ui->postpressCirculationComboBox->currentIndex() == -1)
        return;

    PasportBase::Postpress p;
    QModelIndex idx = postpressCirculationList.index(index, tPostpress::price, QModelIndex());
    p.price = postpressCirculationList.data(idx).toDouble();
    if (p.price < 0)
    {
        ui->postpressCirculationComboBox->setCurrentIndex(-1);
        return;
    }
    idx = postpressCirculationList.index(index, tPostpress::id, QModelIndex());
    p.id = postpressCirculationList.data(idx).toInt();

    unsigned long long i = 0;
    for (i = 0; i < zakaz_ofs->postpressCirculation.size(); ++i)
        if (zakaz_ofs->postpressCirculation[i].id == p.id)
            break;

    if (i != zakaz_ofs->postpressCirculation.size())
    {
        emit postpressCirculationModel->dataChanged(postpressCirculationModel->index(i, 3),
                                                    postpressCirculationModel->index(i, 4));
        ui->postpressCirculationTableView->setFocus();
        ui->postpressCirculationTableView->selectRow(i);
        ui->postpressCirculationComboBox->setCurrentIndex(-1);
        return;
    }
    idx = postpressCirculationList.index(index, tPostpress::name, QModelIndex());
    p.name = postpressCirculationList.data(idx).toString();
    idx = postpressCirculationList.index(index, tPostpress::price, QModelIndex());
    p.price = postpressCirculationList.data(idx).toDouble();

    postpressCirculationModel->insertRows(p.id, p.name, p.price, 1);
    ui->postpressCirculationComboBox->setCurrentIndex(-1);
    ui->postpressCirculationTableView->setFocus();
    ui->postpressCirculationTableView->selectRow(i);

	costRefresh();
}


void ZOfsetDialog::on_addPostpressCirculationButton_clicked()
{
    if (zakaz_ofs->postpressCirculation.size() == 0)
        return;

    int i = ui->postpressCirculationTableView->currentIndex().row();
    ++zakaz_ofs->postpressCirculation[i].count;
    zakaz_ofs->postpressCirculation[i].cost = zakaz_ofs->postpressCirculation[i].price * zakaz_ofs->postpressCirculation[i].count;
    emit postpressCirculationModel->dataChanged(postpressCirculationModel->index(i, 3),
                                                postpressCirculationModel->index(i, 4));
    ui->postpressCirculationTableView->setFocus();
    ui->postpressCirculationTableView->selectRow(i);

	costRefresh();

	return;
}


void ZOfsetDialog::on_removeCirculationPostpressButton_clicked()
{
    if (!postpressCirculationModel->rowCount())
        return;

    int i = ui->postpressCirculationTableView->currentIndex().row();
    if (zakaz_ofs->postpressCirculation[i].count > 1)
    {
        --zakaz_ofs->postpressCirculation[i].count;
        zakaz_ofs->postpressCirculation[i].cost = zakaz_ofs->postpressCirculation[i].price * zakaz_ofs->postpressCirculation[i].count;
        emit postpressCirculationModel->dataChanged(postpressCirculationModel->index(i, 3),
                                                    postpressCirculationModel->index(i, 4));
        ui->postpressCirculationTableView->setFocus();
        ui->postpressCirculationTableView->selectRow(i);

		costRefresh();

		return;
    }

    postpressCirculationModel->removeRows(i);
    ui->postpressCirculationTableView->setFocus();
    if (postpressCirculationModel->rowCount())
        ui->postpressCirculationTableView->selectRow(i);

	costRefresh();
}


void ZOfsetDialog::on_infoTextEdit_textChanged()
{
    zakaz_ofs->setAdditionInfo(ui->infoTextEdit->toPlainText());
}


void ZOfsetDialog::on_putToDBButton_clicked()
{
	if (copyOrEdit)
	{
		if (!preOrNew)
		{
			if (zakaz_ofs->putToDb(db, tableName, zakaz_ofs->getPrintingMachine().name, newOrPre::newPasport))
				this->close();
			emit refreshPasportTable();
		}
		else
		{
			if (zakaz_ofs->putToDb(db, tableName, zakaz_ofs->getPrintingMachine().name, newOrPre::prePasport))
				this->close();
			emit refreshPasportTable();
		}
	}
	else on_reSaveButton_clicked();
}


void ZOfsetDialog::on_reSaveButton_clicked()
{
    int id = record.value(tPasportOfset::id).toInt();
	if (zakaz_ofs->updateToDb(db, tableName, id))
    {
        emit refreshPasportTableAfterEdit(id);
		emit refreshAll();
        this->close();
    }

}


void ZOfsetDialog::on_productionlineEdit_textChanged(const QString &arg1)
{
    zakaz_ofs->setProduction(ui->productionlineEdit->text());
}


void ZOfsetDialog::on_pantoneListLineEdit_textChanged(const QString &arg1)
{
    zakaz_ofs->setPantoneList(ui->pantoneListLineEdit->text());
}

void ZOfsetDialog::postpressParsing(const QString s,
									PostpressTableModel *model,
									const QSqlQueryModel &list,
									std::vector<PasportBase::Postpress> &setTo)
{
    int i = 0;
    while (i < s.size())
    {
        QString idBuf;
        QString countBuf;
		QString priceBuf;

        if(s[i] == '(')
        {
            ++i;
            while (s[i] != ',')
            {
                idBuf += s[i];
                ++i;
            }
            ++i;
			while (s[i] != ',')
            {
                countBuf += s[i];
                ++i;
            }
            ++i;
			while (s[i] != ')')
			{
				priceBuf += s[i];
				++i;
			}
			++i;

			PasportBase::Postpress p;
			p.setId(idBuf.toInt(), list);
            p.setCount(countBuf.toInt());
			p.setPrice(priceBuf.toDouble());
			setTo.push_back(p);
        }
        else ++i;
    }
}

void ZOfsetDialog::on_savePreButton_clicked()
{
	if (zakaz_ofs->putToDb(db, tableName, zakaz_ofs->getPrintingMachine().name, newOrPre::prePasport))
		this->close();

	emit refreshPasportTable();
}

void ZOfsetDialog::on_calculatePushButton_clicked()
{
	costRefresh();
}


void ZOfsetDialog::on_paperPriceSpinBox_valueChanged(double arg1)
{
	zakaz_ofs->setPaperPrice(ui->paperPriceSpinBox->value());
	costRefresh();
}

void ZOfsetDialog::costRefresh()
{
	ui->price->setText(tlib::conformCostString((zakaz_ofs->*calculateProductionPrice)()) + " руб.");
}

