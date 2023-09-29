#include "include/typography.h"

#include <QMessageBox>
#include <QSqlError>
#include <QSqlRecord>

extern QSqlDatabase db;

// ------------------------------------------------------------
// конструктор класса

PasportBase::PasportBase()
{
}

void PasportBase::setInstCount(long long count)
{
    instancesCount = count;
    setSheetsCount();
}

// ------------------------------------------------------------
// расчет количества экземпляров на листе

void PasportBase::instPerSheets()
{
    if (instanceSize.preTrimHght == 0 || instanceSize.preTrimWdth == 0
          || stockSize.printAreaWidth == 0 || stockSize.printAreaHeight == 0)
    {
        instancesPerSheet = 0;
        setSheetsCount();
        return;
    }

    struct count
	{
		int hor;
		int ver;
		int dop_hor;
		int dop_ver;
		int count;
	};
	count var1{ 0, 0, 0, 0, 0 };
	count var2{ 0, 0, 0, 0, 0 };

	// 1-й вариант расположения
    var1.hor = stockSize.printAreaWidth / instanceSize.preTrimWdth;                                                    // количество по горизонтале
    var1.ver = stockSize.printAreaHeight / instanceSize.preTrimHght;                                                    // количество по вертикале
	var1.dop_hor = ((stockSize.printAreaWidth - instanceSize.preTrimWdth * var1.hor)
					/ instanceSize.preTrimHght)
				   * (stockSize.printAreaHeight / instanceSize.preTrimWdth);                                                       // количество перевернутых на 90 гр экземпляров на оставшемся месте слева
	var1.dop_ver = ((stockSize.printAreaHeight - instanceSize.preTrimHght * var1.ver)
					/ instanceSize.preTrimWdth)
				   * (stockSize.printAreaWidth / instanceSize.preTrimHght);                                                       // количество перевернутых на 90 гр экземпляров на оставшемся месте внизу
	var1.count = var1.hor * var1.ver + var1.dop_hor + var1.dop_ver;

	// 2-й вариант расположения
    var2.hor = stockSize.printAreaHeight / instanceSize.preTrimWdth;                                                	// количество по горизонтале
    var2.ver = stockSize.printAreaWidth / instanceSize.preTrimHght;                                                	// количество по вертикале
	var2.dop_hor = ((stockSize.printAreaHeight - instanceSize.preTrimWdth * var2.hor)
					/ instanceSize.preTrimHght)
				   * (stockSize.printAreaWidth / instanceSize.preTrimWdth);                                                   	// количество перевернутых на 90 гр экземпляров на оставшемся месте слева
	var2.dop_ver = ((stockSize.printAreaWidth - instanceSize.preTrimHght * var2.ver)
					/ instanceSize.preTrimWdth)
				   * (stockSize.printAreaHeight / instanceSize.preTrimHght);                                                       // количество перевернутых на 90 гр экземпляров на оставшемся месте внизу
	var2.count = var2.hor * var2.ver + var2.dop_hor + var2.dop_ver;

    var1.count > var2.count ? instancesPerSheet = var1.count : instancesPerSheet = var2.count;

    if (instancesPerSheet > 0)
    {
        sheetsCount = instancesCount / instancesPerSheet;
        if (instancesCount % instancesPerSheet) ++sheetsCount;
    }

    setSheetsCount();
}

// ------------------------------------------------------------
// ввод размера бумаги

void PasportBase::setStockSize(int x, int y)
{
    stockSize.width = x;
    stockSize.height = y;

    if (x > printingMachine.techFieldWdth && y > printingMachine.techFieldHght)
    {
        stockSize.printAreaWidth = x - printingMachine.techFieldWdth;
        stockSize.printAreaHeight = y - printingMachine.techFieldHght;
    }
    else
    {
        stockSize.printAreaWidth = 0;
        stockSize.printAreaHeight = 0;
    }

    instPerSheets();
}

// ------------------------------------------------------------
// установить размер обрезного поля

void PasportBase::setTrimVal(int trim)
{
    instanceSize.trimVal = trim;

    setInstPreTrim();

    instPerSheets();
}

settings::WorkTypes PasportBase::getWorkType(const QString name,
                                             QObject *parent)
{
    QSqlTableModel wtModel(parent, db);
    wtModel.setTable("work_types");
    wtModel.select();
    QModelIndex idx;
    int i;
    for (i = 0; i < wtModel.rowCount(); ++i)
    {
        idx = wtModel.index(i, tWorkTypes::name);
        if (wtModel.data(idx).toString() == name)
            break;
    }
    settings::WorkTypes wt;
    if (i >= wtModel.rowCount())
    {
        return wt;
    }

    QSqlRecord rec = wtModel.record(i);
    wt.id = rec.value(tWorkTypes::id).toInt();
    wt.name = rec.value(tWorkTypes::name).toString();
    wt.type = rec.value(tWorkTypes::type).toString();
    wt.tableName = rec.value(tWorkTypes::tableName).toString();
    wt.sectionCount = rec.value(tWorkTypes::sectionCount).toInt();
    wt.fitting = rec.value(tWorkTypes::fitting).toInt();
    wt.maxPaperWidth = rec.value(tWorkTypes::maxPaperWidth).toInt();
    wt.maxPaperHeight = rec.value(tWorkTypes::maxPaperHeight).toInt();
    wt.techFieldWdth = rec.value(tWorkTypes::techFieldWdth).toInt();
    wt.techFieldHght = rec.value(tWorkTypes::techFieldHght).toInt();
	wt.fittingTime = rec.value(tWorkTypes::fittingTime).toDouble();
	wt.printSpeed = rec.value(tWorkTypes::printSpeed).toInt();

    return wt;
}

void PasportBase::setSupplies()
{
	QSqlQuery query(db);
	query.prepare("SELECT val FROM supplies WHERE name = 'platePrice'");
	query.exec();
	query.first();
	platePrice = query.value(0).toDouble();

	query.prepare("SELECT val FROM supplies WHERE name = 'paintWeightPerSheet'");
	query.exec();
	query.first();
	paintWeightPerSheet = query.value(0).toDouble();

	query.prepare("SELECT val FROM supplies WHERE name = 'paintPricePerSheet'");
	query.exec();
	query.first();
	paintPricePerSheet = query.value(0).toDouble();

	query.prepare("SELECT val FROM supplies WHERE name = 'pantonPricePerSheet'");
	query.exec();
	query.first();
	pantonPricePerSheet = query.value(0).toDouble();

	query.prepare("SELECT val FROM supplies WHERE name = 'ofsLakPricePerSheet'");
	query.exec();
	query.first();
	ofsLakPricePerSheet = query.value(0).toDouble();

	query.prepare("SELECT val FROM supplies WHERE name = 'printHourPriceOfset'");
	query.exec();
	query.first();
	printHourPriceOfset = query.value(0).toDouble();

	query.prepare("SELECT val FROM supplies WHERE name = 'printPriceXerox'");
	query.exec();
	query.first();
	printPriceXerox = query.value(0).toDouble();

	qDebug() << platePrice
			 << paintWeightPerSheet
			 << paintPricePerSheet
			 << pantonPricePerSheet
			 << ofsLakPricePerSheet
			 << printHourPriceOfset
			 << printPriceXerox;
}

double PasportBase::calculateOfsetProductionPrice()
{
	qDebug() << 6;
	if (!(paperPrice && sheetsCount && colorCount.runs))
	{
		productionPrice = 0;
		return productionPrice;
	}

	double postpressCost = 0;
	for (int i = 0; i < postpress.size(); ++i)
		postpressCost += postpress[i].cost;
	postpressCost *= instancesCount;
	qDebug() << "postpressCost =" << postpressCost;

	double postpressSheetCost = 0;
	for (int i = 0; i < postpressSheet.size(); ++i)
		postpressSheetCost += postpressSheet[i].cost;
	postpressSheetCost *= sheetsCount;
	qDebug() << "postpressSheetCost =" << postpressSheetCost;

	double postpressCirculationCost = 0;
	for (int i = 0; i < postpressCirculation.size(); ++i)
		postpressCirculationCost += postpressCirculation[i].cost;
	qDebug() << "postpressCirculationCost =" << postpressCirculationCost;

	double paperCost = paperPrice * overallSheetsCount;
	qDebug() << "paperCost =" << paperCost;

	double platesCost = (colorCount.face + colorCount.back) * platePrice;
	qDebug() << "platesCost =" << platesCost;

	double paintsCost = (((colorCount.cmykFace + colorCount.cmykBack) * paintPricePerSheet)
						+ ((colorCount.pantoneFace + colorCount.pantoneBack) * pantonPricePerSheet)
						+ ((colorCount.ofsLakFace + colorCount.ofsLakBack) * ofsLakPricePerSheet))
						* overallSheetsCount;
	qDebug() << "paintsCost =" << paintsCost;

	if (colorCount.runs != 0 && printingMachine.printSpeed != 0)
	{
		printTime = printingMachine.fittingTime * (colorCount.face + colorCount.back)/*colorCount.runs*/ +
					(double(overallSheetsCount) / printingMachine.printSpeed * colorCount.runs) * 2;
	}
	else printTime = 0;
	qDebug() << "printTime =" << printTime;

	double printingTimeCost = printTime * printHourPriceOfset;
	qDebug() << "printingTimeCost =" << printingTimeCost;


	productionPrice = paperCost + platesCost + paintsCost + printingTimeCost +
					  postpressCost + postpressSheetCost + postpressCirculationCost;
	qDebug() << "productionPrice =" << productionPrice;

	return productionPrice;
}

double PasportBase::calculateXeroxProductionPrice()
{
	qDebug() << 6;
	if (!(paperPrice && sheetsCount && colorCount.runs))
	{
		productionPrice = 0;
		return productionPrice;
	}

	double postpressCost = 0;
	for (int i = 0; i < postpress.size(); ++i)
		postpressCost += postpress[i].cost;
	postpressCost *= instancesCount;
	qDebug() << "postpressCost =" << postpressCost;

	double postpressSheetCost = 0;
	for (int i = 0; i < postpressSheet.size(); ++i)
		postpressSheetCost += postpressSheet[i].cost;
	postpressSheetCost *= sheetsCount;
	qDebug() << "postpressSheetCost =" << postpressSheetCost;

	double postpressCirculationCost = 0;
	for (int i = 0; i < postpressCirculation.size(); ++i)
		postpressCirculationCost += postpressCirculation[i].cost;
	qDebug() << "postpressCirculationCost =" << postpressCirculationCost;

	double paperCost = paperPrice * overallSheetsCount;
	qDebug() << "paperCost =" << paperCost;

	productionPrice = paperCost + colorCount.runs * printPriceXerox * overallSheetsCount +
					  postpressCost + postpressSheetCost + postpressCirculationCost;
	qDebug() << "colorCount.runs =" << colorCount.runs;
	qDebug() << "printPriceXerox =" << printPriceXerox;
	qDebug() << "productionPrice =" << productionPrice;

	return productionPrice;
}

bool PasportOfset::putToDb(const QSqlDatabase &db,
                           QString tableName,
						   QString workType,
						   char pasType)
{
    if (!isReadyToSave())
        return false;

    QSqlQuery query(db);
	QString postpressStr;
	conformPostpressString(postpress, postpressStr);

	QString postpressSheetStr;
	conformPostpressString(postpressSheet, postpressSheetStr);

	QString postpressCirculationStr;
	conformPostpressString(postpressCirculation, postpressCirculationStr);

    QString clientName;
    if (client.company.size())
    {
        clientName = client.company;
        if (client.person.size())
            clientName += ", " + client.person;
    }
    else clientName = client.person;

    query.prepare("INSERT INTO "
                  + tableName +
                  " (client, issue_date, production, paper, inst_wdth, inst_hght, trim_val, face_cmyk, face_ofs_lak, face_pantone, "
                  "back_cmyk, back_ofs_lak, back_pantone, pantone_list, inst_count, addition_info, postpress, postpress_sheet, postpress_circulation, "
				  "client_name, work_type, creation_date, id, paper_price, cost, owner)"
				  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(client.id);
    query.addBindValue(issueDate);
    query.addBindValue(production);
    query.addBindValue(stockSize.id);
    query.addBindValue(instanceSize.width);
    query.addBindValue(instanceSize.height);
    query.addBindValue(instanceSize.trimVal);
    query.addBindValue(colorCount.cmykFace);
    query.addBindValue(colorCount.ofsLakFace);
    query.addBindValue(colorCount.pantoneFace);
    query.addBindValue(colorCount.cmykBack);
    query.addBindValue(colorCount.ofsLakBack);
    query.addBindValue(colorCount.pantoneBack);
    query.addBindValue(colorCount.pantonList);
    query.addBindValue(instancesCount);
    query.addBindValue(additionInfo);
    query.addBindValue(postpressStr);
    query.addBindValue(postpressSheetStr);
    query.addBindValue(postpressCirculationStr);
    query.addBindValue(clientName);
    query.addBindValue(workType);
    query.addBindValue(creationDate);

	long long mId = 0;
	if (pasType == newOrPre::newPasport)
		mId = newPasportId(db, "pasport_ofset");
	else
		mId = prePasportId(db, "pasport_ofset");

	if (mId)
		query.addBindValue(mId);
	else
	{
		QMessageBox msg;
		msg.setText("Ошибка при назначении номера заказа! \n Попробуйте еще раз. " + query.lastError().text());
		msg.exec();
		return false;
	}

	query.addBindValue(paperPrice);
	query.addBindValue(productionPrice);
	query.addBindValue(user.name.fullName);

	if (!query.exec())
    {
        QMessageBox msg;
        msg.setText("Что-то пошло не так! " + query.lastError().text());
        msg.exec();
        return false;
    }
    return true;
}


bool PasportOfset::updateToDb(const QSqlDatabase &db,
                              QString tableName,
                              int id)
{
    if (!isReadyToSave())
        return false;

    QSqlQuery query(db);

	QString postpressStr;
	conformPostpressString(postpress, postpressStr);

	QString postpressSheetStr;
	conformPostpressString(postpressSheet, postpressSheetStr);

	QString postpressCirculationStr;
	conformPostpressString(postpressCirculation, postpressCirculationStr);

    QString clientName;
    if (client.company.size())
    {
        clientName = client.company;
        if (client.person.size())
            clientName += ", " + client.person;
    }
    else clientName = client.person;

    query.prepare("UPDATE "
                  + tableName +
                  " SET client=:cnt, issue_date=:dat, production=:prod, paper=:pap, inst_wdth=:iw, inst_hght=:ih, trim_val=:tv, face_cmyk=:fcmyk, face_ofs_lak=:fol, face_pantone=:fp, "
                  "back_cmyk=:bcmyk, back_ofs_lak=:bol, back_pantone=:bp, pantone_list=:pl, inst_count=:ic, addition_info=:ai, postpress=:pp, postpress_sheet=:psh, postpress_circulation=:pps, "
				  "client_name=:cntn, paper_price=:pprice, cost=:prodprice "
                  "WHERE id = " + QVariant(id).toString());

    query.bindValue(":cnt", client.id);
    query.bindValue(":dat", issueDate);
    query.bindValue(":prod", production);
    query.bindValue(":pap", stockSize.id);
    query.bindValue(":iw", instanceSize.width);
    query.bindValue(":ih", instanceSize.height);
    query.bindValue(":tv", instanceSize.trimVal);
    query.bindValue(":fcmyk", colorCount.cmykFace);
    query.bindValue(":fol", colorCount.ofsLakFace);
    query.bindValue(":fp", colorCount.pantoneFace);
    query.bindValue(":bcmyk", colorCount.cmykBack);
    query.bindValue(":bol", colorCount.ofsLakBack);
    query.bindValue(":bp", colorCount.pantoneBack);
    query.bindValue(":pl", colorCount.pantonList);
    query.bindValue(":ic", instancesCount);
    query.bindValue(":ai", additionInfo);
    query.bindValue(":pp", postpressStr);
    query.bindValue(":psh", postpressSheetStr);
    query.bindValue(":pps", postpressCirculationStr);
    query.bindValue(":cntn", clientName);
	query.bindValue(":pprice", paperPrice);
	query.bindValue(":prodprice", productionPrice);

    if (!query.exec())
    {
        QMessageBox msg;
        msg.setText("Что-то пошло не так! " + query.lastError().text());
        msg.exec();
        return false;
    }
    return true;
}

// ------------------------------------------------------------
// установить дообрезной размер экземпляра

void PasportBase::setInstPreTrim()
{
    if (instanceSize.width && instanceSize.height)
    {
        instanceSize.preTrimWdth = instanceSize.width + instanceSize.trimVal * 2;
        instanceSize.preTrimHght = instanceSize.height + instanceSize.trimVal * 2;
    }
    else
    {
        instanceSize.preTrimWdth = 0;
        instanceSize.preTrimHght = 0;
    }
}

void PasportBase::setFitting()
{
    if (colorCount.face > colorCount.back)
        fittingCount = (colorCount.face -
                         colorCount.ofsLakFace) *
                         printingMachine.fitting;
    else
        fittingCount = (colorCount.back -
                         colorCount.ofsLakBack) *
                         printingMachine.fitting;

    setOverallSheetsCount();
}

void PasportBase::setSheetsCount()
{
    if (instancesPerSheet != 0)
    {
        sheetsCount = instancesCount / instancesPerSheet;
        if (instancesCount % instancesPerSheet)
            ++sheetsCount;
    }
    else sheetsCount = 0;

    setOverallSheetsCount();
}

// ------------------------------------------------------------
// ввод размера экземпляра

void PasportBase::setInstSize(int x, int y)
{
    instanceSize.width = x;
    instanceSize.height = y;

    setInstPreTrim();

    instPerSheets();
}

// ------------------------------------------------------------
// устанавливает цветность тиража

void PasportBase::setColorCount(int cmykFace, int pantoneFace, int ofsLakFace,
                                int cmykBack, int pantoneBack, int ofsLakBack)
{
    int face = cmykFace + pantoneFace + ofsLakFace,
        back = cmykBack + pantoneBack + ofsLakBack,
        face_runs = 0, back_runs = 0;

    face_runs = face / printingMachine.sectionCount;
    if (face % printingMachine.sectionCount) ++face_runs;

    back_runs = back / printingMachine.sectionCount;
    if (back % printingMachine.sectionCount) ++back_runs;

	colorCount.face = face;
	colorCount.back = back;
	colorCount.runs = face_runs + back_runs;

    setFitting();
}

// ------------------------------------------------------------
// конструктор класса Pasport_Ofset
PasportOfset::PasportOfset()
{
    stockSize = { -1, 0, 0, 0, 0 };
    instanceSize = { 0, 0, 1, 0, 0 };
    instancesPerSheet = 0;
    sheetsCount = 0;
    instancesCount = 0;
    colorCount = { 0, 0, 0, 0, 0, 0, 0, 0, 0, "" };
    paperPrice = 0;
    fittingCount = 0;
    sheetsCount = 0;
    overallSheetsCount = 0;
    issueDate = QDate::currentDate();
}


PasportOfset::PasportOfset(settings::WorkTypes pm)
{
    printingMachine = pm;
    stockSize = { -1, 0, 0, 0, 0 };
    instanceSize = { 0, 0, 0, 0, 0 };
    instancesPerSheet = 0;
    sheetsCount = 0;
    instancesCount = 0;
    colorCount = { 0, 0, 0, 0, 0, 0, 0, 0, 0, "" };
    paperPrice = 0;
    fittingCount = 0;
    sheetsCount = 0;
    overallSheetsCount = 0;
    issueDate = QDate::currentDate();
}

bool PasportOfset::isReadyToSave()
{
    // проверяем заказчика
    if (client.id < 0)
    {
        QMessageBox msg;
        msg.setText("Выберите заказчика.");
        msg.exec();
        return false;
    }

    // проверяем наименование продукции
    if (!production.size())
    {
        QMessageBox msg;
        msg.setText("Введите наименование продукции.");
        msg.exec();
        return false;
    }
    // проверяем бумагу
    if (stockSize.id < 0)
    {
        QMessageBox msg;
        msg.setText("Выберите бумагу.");
        msg.exec();
        return false;
    }

    // проверяем размер бумаги
    if(stockSize.width > printingMachine.maxPaperWidth || stockSize.height > printingMachine.maxPaperHeight)
        if(stockSize.width > printingMachine.maxPaperHeight || stockSize.height > printingMachine.maxPaperWidth)
        {
            QMessageBox msg;
            msg.setText("Размер бумаги превышает максимально допустимый."
                        "Выберите другую бумагу.");
            msg.exec();
            return false;
        }

    // проверяем размер экземпляра
    if (instanceSize.height == 0 || instanceSize.width == 0)
    {
        QMessageBox msg;
        msg.setText("Введите размер экземпляра.");
        msg.exec();
        return false;
    }
    else if (instancesPerSheet == 0)
    {
        QMessageBox msg;
        msg.setText("Размер экземпляра превышает допустимый для выбранной бумаги.");
        msg.exec();
        return false;
    }

    // проверяем цветность
    if (colorCount.face == 0 && colorCount.back == 0)
    {
        QMessageBox msg;
        msg.setText("Введите цветность.");
        msg.exec();
        return false;
    }

    // проверяем тираж
    if (instancesCount < 1)
    {
        QMessageBox msg;
        msg.setText("Введите количество экземпляров.");
        msg.exec();
        return false;
    }

	return true;
}

long long PasportOfset::newPasportId(const QSqlDatabase &db, const QString tableName)
{
	QSqlQuery queryId(db);
	queryId.prepare("SELECT MAX(id) FROM " + tableName);
	if (!queryId.exec())
		return 0;

	queryId.first();
	long long mId = queryId.value(0).toLongLong();
	return ++mId;
}

long long PasportOfset::prePasportId(const QSqlDatabase &db, const QString tableName)
{
	QSqlQuery queryId(db);
	queryId.prepare("SELECT MIN(id) FROM " + tableName);
	if (!queryId.exec())
		return 0;

	queryId.first();
	long long mId = queryId.value(0).toLongLong();
	if (mId >= 0)
		return -1;
	return --mId;
}

void PasportOfset::conformPostpressString(const std::vector <PasportBase::Postpress> &postpressVector,
										  QString &postpressString)
{
	postpressString = "{";
	for (unsigned long long i = 0; i < postpressVector.size(); ++i)
	{
		postpressString += "\"(";
		postpressString += QString::number(postpressVector[i].id);
		postpressString += ',';
		postpressString += QString::number(postpressVector[i].count);
		postpressString += ',';
		postpressString += QString::number(postpressVector[i].price);
		i == postpressVector.size() - 1 ? postpressString += ")\"" : postpressString += ")\",";
	}
	postpressString += "}";
}



QVector <QString> parsingFilterString(const QString& s) // разбивает строку фильтра на подстроки
{
    QVector<QString> v;
    QString buf = 0;

    for (int i = 0; i < s.size(); ++i)
    {
        if (s[i] != ' ')
        {
            buf += s[i];
        }
        else if (!buf.isEmpty())
        {
            v.push_back(buf);
            buf = "";
        }
    }
    if (!buf.isEmpty())
        v.push_back(buf);
    return v;
}

long long maxId(QSqlTableModel *model, int column)
{
	long long max = 0;
	long long row = 0;
	long long curId = 0;
	for (long long i = 0; i < model->rowCount(); ++i)
    {
		curId = model->data(model->index(i, column)).toInt();
        if (curId > max)
        {
            max = curId;
            row = i;
        }
    }
    return row;
}

long long minId(QSqlTableModel *model, int column)
{
	long long min = model->data(model->index(0, column)).toInt();
	long long row = 0;
	long long curId = 0;
	for (long long i = 0; i < model->rowCount(); ++i)
	{
		curId = model->data(model->index(i, column)).toInt();
		if (curId < min)
		{
			min = curId;
			row = i;
		}
	}
	return row;
}


long long curRow(int id, QSqlTableModel *model, int column)
{
	for (long long i = 0; i < model->rowCount(); ++i)
    {
		if (id == model->data(model->index(i, column)).toInt())
        {
            return i;
        }
    }
    return 0;
}

void PasportBase::Postpress::setId(int id, const QSqlQueryModel &model)
{
    QModelIndex idx;
    int i;
    for(i = 0; i < model.rowCount(); ++i)
    {
        idx = model.index(i, 0, QModelIndex());
        if(model.data(idx).toInt() == id)
            break;
    }
    this->id = id;
    idx = model.index(i, tPostpress::name);
    this->name = model.data(idx).toString();
    idx = model.index(i, tPostpress::price);
    this->price = model.data(idx).toDouble();
}

void PasportBase::Postpress::setCount(int cnt)
{
    count = cnt;
	cost = count * price;
}

void PasportBase::Postpress::setPrice(double prc)
{
	price = prc;
	cost = count * price;
}


settings::WorkTypes getPrintParams(const QString name)
{
    QSqlTableModel wtModel(nullptr, db);
	wtModel.setTable("work_types");
	wtModel.select();
	QModelIndex idx;
	int i;
	for (i = 0; i < wtModel.rowCount(); ++i)
	{
		idx = wtModel.index(i, tWorkTypes::referens);
		if (wtModel.data(idx).toString() == name)
			break;
	}
	settings::WorkTypes wt;
	if (i >= wtModel.rowCount())
	{
		return wt;
	}

	QSqlRecord rec = wtModel.record(i);
	wt.id = rec.value(tWorkTypes::id).toInt();
	wt.name = rec.value(tWorkTypes::name).toString();
	wt.type = rec.value(tWorkTypes::type).toString();
	wt.tableName = rec.value(tWorkTypes::tableName).toString();
	wt.sectionCount = rec.value(tWorkTypes::sectionCount).toInt();
	wt.fitting = rec.value(tWorkTypes::fitting).toInt();
	wt.maxPaperWidth = rec.value(tWorkTypes::maxPaperWidth).toInt();
	wt.maxPaperHeight = rec.value(tWorkTypes::maxPaperHeight).toInt();
	wt.techFieldWdth = rec.value(tWorkTypes::techFieldWdth).toInt();
	wt.techFieldHght = rec.value(tWorkTypes::techFieldHght).toInt();
	wt.fittingTime = rec.value(tWorkTypes::fittingTime).toDouble();
	wt.printSpeed = rec.value(tWorkTypes::printSpeed).toInt();

	return wt;
}
