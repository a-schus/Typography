#include "include/filtertable.h"
#include "ui_filtertable.h"

// -------------------------------------------------------------------------------------------------
// FilterTable
// -------------------------------------------------------------------------------------------------
FilterTable::FilterTable(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::FilterTable)
{
	ui->setupUi(this);
	hLayout = new QHBoxLayout(this);
	hLayout->setContentsMargins(0, 0, 0, 0);
}

FilterTable::~FilterTable()
{
	delete ui;
}

void FilterTable::addCheckBox(QString name, QString filterString)
{
	TCheckBox* box = new TCheckBox(name, filterString, this);
	QObject::connect(box, &TCheckBox::clicked,
					 this, &FilterTable::formFilterString);

	box->setChecked(false);
	checkBox.push_back(box);
	int i = checkBox.size() - 1;
	hLayout->addWidget(checkBox[i]);
}

void FilterTable::addLineEdit()
{
	if (isLineEdit)	return;

	isLineEdit = true;
	lineEdit = new QLineEdit(this);
	QObject::connect(lineEdit, &QLineEdit::textChanged,
					 this, &FilterTable::whenLineEditChanged);

	lineEdit->setPlaceholderText(" Поиск");
	lineEdit->setClearButtonEnabled(true);

	hLayout->addWidget(lineEdit);
}

void FilterTable::clearLineEdit()
{
	if (isLineEdit)
		lineEdit->setText("");
}

QVector<QString> FilterTable::parsingFilterString(const QString &s)
{
	QVector<QString> v;
	QString buf = 0;

	foreach (QChar c, s)
	{
		if (c != ' ')
		{
			buf += c;
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

void FilterTable::formFilterString()
{
	QString checkBoxString;
	QString lineEditString;
	filter = "";

	// Формируем фильтр из чекбоксов
	if (checkBox.size())
	{
		foreach (TCheckBox* ch, checkBox)
		{
			if (ch->isChecked())
			{
				if (!checkBoxString.size())
					checkBoxString += ch->filterString;
				else
					checkBoxString += " or " + ch->filterString;
			}
		}
		if (checkBoxString.size())
			checkBoxString = "(" + checkBoxString + ")";
	}

	// Формируем фильтр из строки поиска
	QVector<QString> parsWhere = parsingFilterString(lineEdit->text());
	if(parsWhere.size())
	{
		lineEditString = "(text(id) LIKE '%" + parsWhere[0] + "%' OR "
				 "LOWER(client_name) LIKE LOWER('%" + parsWhere[0] + "%') OR "
				 "LOWER(production) LIKE LOWER('%" + parsWhere[0] + "%'))";
		for(int i = 1; i < parsWhere.size(); ++i)
		{
			lineEditString += " and (text(id) LIKE '%" + parsWhere[i] + "%' OR "
					 "LOWER(client_name) LIKE LOWER('%" + parsWhere[i] + "%') OR "
					 "LOWER(production) LIKE LOWER('%" + parsWhere[i] + "%'))";
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

void FilterTable::whenLineEditChanged(QString str)
{
	formFilterString();
}

// -------------------------------------------------------------------------------------------------
// TCheckBox
// -------------------------------------------------------------------------------------------------
TCheckBox::TCheckBox(QString name, QString filterString, QWidget *parent)
	: QCheckBox(parent)
{
	TCheckBox::name = name;
	TCheckBox::filterString = filterString;
	setText(name);
}

TCheckBox::~TCheckBox()
{

}
