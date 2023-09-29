#ifndef FILTERTABLE_H
#define FILTERTABLE_H

#include <QCheckBox>
#include <QHBoxLayout>
#include <QVector>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>

// -------------------------------------------------------------------------------------------------
// TCheckBox
// -------------------------------------------------------------------------------------------------
class TCheckBox : public QCheckBox
{
	Q_OBJECT

public:
	QString filterString;
	QString name;

	TCheckBox(QString name, QString filterString, QWidget *parent = nullptr);
	~TCheckBox();
};

// -------------------------------------------------------------------------------------------------
// FilterTable
// -------------------------------------------------------------------------------------------------
QT_BEGIN_NAMESPACE
namespace Ui { class FilterTable; }
QT_END_NAMESPACE

class FilterTable : public QWidget
{
	Q_OBJECT

protected slots:
	virtual void formFilterString();
	void whenLineEditChanged(QString str);

signals:
	void filterChanged(QString filterString);

public:
	FilterTable(QWidget *parent = nullptr);
	~FilterTable();

	void addCheckBox(QString name, QString filterString);
	void addLineEdit();
	void clearLineEdit();

protected:
	QVector <QString> parsingFilterString(const QString& s);

protected:
	Ui::FilterTable *ui;

	QHBoxLayout* hLayout;
	QLabel* label;
	QLineEdit* lineEdit;
	bool isLineEdit = false;
	QVector<TCheckBox*> checkBox;

	QString filter;
};
#endif // FILTERTABLE_H
