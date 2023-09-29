#ifndef PASPORTBASEVIEWWIDGET_H
#define PASPORTBASEVIEWWIDGET_H

#include <QMenu>
#include <QWidget>
#include <QSortFilterProxyModel>
#include "typography.h"
#include "zofsetdialog.h"
#include "filtertable.h"
#include "printreport.h"

extern QSqlDatabase db;
// -----------------------------------------------------------------------------------------------
// TPasportModel
// -----------------------------------------------------------------------------------------------
class TPasportModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	enum FilterCondition
	{
		Equal = 0,
		NotEqual,
		Smaller,
		SmallerOrEqual,
		Lager,
		LagerOrEqual
	};

	enum FilterOperator
	{
		AND = 0,
		OR
	};

	struct FilterItem
	{
		int column;
		QString filterString;
		FilterCondition condition;
		FilterOperator filterOperator;
	};

	int preOrNew;

private:
	QList<FilterItem> filterList;


	// QAbstractItemModel interface
public:
	explicit TPasportModel(int pre, QWidget *parent = nullptr, const QSqlDatabase &db = QSqlDatabase());

	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	QList<FilterItem> filter() const
	{return filterList;}

	void addFilter(const FilterItem& filterItem);

	void eraseFilter(const FilterItem &filterItem);

	void setFilter(QList<FilterItem> filter)
	{
		filterList = filter;
		invalidate();
	}

	// QSortFilterProxyModel interface
protected:
	virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};

// -----------------------------------------------------------------------------------------------
// PasportBaseViewWidget
// -----------------------------------------------------------------------------------------------
namespace Ui {
class PasportBaseViewWidget;
}

class PasportBaseViewWidget : public QWidget
{
    Q_OBJECT

public:
	explicit PasportBaseViewWidget(int pre, QWidget *parent = nullptr);

	explicit PasportBaseViewWidget(QDate date, int pre, QWidget *parent = nullptr);


    ~PasportBaseViewWidget();

	void refresh();

signals:
	void closeAll();
	void refreshAll();

protected:
    void createPasportTableViewContMenu();

private slots:


	void on_pasportOfsetTableView_doubleClicked(const QModelIndex &index);

	void on_editButton_clicked();

	void on_copyButton_clicked();

    void onPaidClicked();

	void onPrintedClicked();

	void onPreperedClicked();

	void onIssuedClicked();

    void onDeleteButtonCliked();

	void recoverPasport();

	void preToNew();

    void on_pasportOfsetTableView_customContextMenuRequested(const QPoint &pos);

	void pasportTableFilterChanged(QString filter);

	void on_heidelbergCheckBox_toggled(bool checked);

	void on_xeroxCheckBox_toggled(bool checked);

	void on_deletedHideRadioButton_toggled(bool checked);

	void on_deletedOnlyRadioButton_toggled(bool checked);

	void on_deletedGroupBox_toggled(bool arg1);

	void printReport();

public slots:
	void refreshTable();

	void on_refreshPushButton_clicked();

	void refreshTableAfterEdit(long long index);

public:
	void refreshCurrentDateTable(long long row);

private:
    Ui::PasportBaseViewWidget *ui;

    ZOfsetDialog *zOfsWin;

	// контекстное меню
	QAction *preToNewAction;
	QMenu *pasportTableViewContMenu;
	QMenu *newSubMenu;
	QAction *newHeidelberg;
	QAction *newXerox;
	QAction *editAction;
	QAction *copyAction;
	QAction *deleteAction;
	QAction *recoverAction;
	QAction *paidAction;
	QAction *printedAction;
	QAction *preparedAction;
	QAction *issuedAction;
	QAction *printAction;
	//------------------

    QString workType;

	FilterTable* pasportTableFilter;

public:
	int preOrNew;

    TPasportModel *pasportOfsetModel;
	QSqlTableModel *pasportOfsetSourseModel;



	QModelIndex curIndex();
};
#endif // PASPORTBASEVIEWWIDGET_H
