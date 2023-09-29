#ifndef PAPERSTABLEWIDGET_H
#define PAPERSTABLEWIDGET_H

#include <QKeyEvent>
#include <QWidget>
#include "paperdialog.h"
#include "pasportbaseviewwidget.h"

// -----------------------------------------------------------------------------------------------
// FilterPapersTable
// -----------------------------------------------------------------------------------------------
namespace Ui { class FilterPapersTable; }

class FilterPapersTable : public FilterTable
{
	Q_OBJECT

public:
	explicit FilterPapersTable(QWidget *parent = nullptr);
	~FilterPapersTable();

	// FilterTable interface
private slots:
	virtual void formFilterString() override;
};

// -----------------------------------------------------------------------------------------------
// TPapersModel
// -----------------------------------------------------------------------------------------------
namespace Ui { class TPapersModel; }

class TPapersModel : public TPasportModel
{
	Q_OBJECT

	// QAbstractItemModel interface
public:
	explicit TPapersModel(QWidget *parent = nullptr);
	~TPapersModel();

	virtual QVariant data(const QModelIndex &index, int role) const override;
};


// -----------------------------------------------------------------------------------------------
// PapersTableWidget
// -----------------------------------------------------------------------------------------------
namespace Ui {
	class PapersTableWidget;
}

class PapersTableWidget : public QWidget
{
	Q_OBJECT

public:
	explicit PapersTableWidget(QWidget *parent = nullptr);
	~PapersTableWidget();

public slots:
	void refresh(int focusRow);

private slots:
	void papersTableFilterChanged(QString filter);

	void on_deletedHideRadioButton_toggled(bool checked);

	void on_deletedOnlyRadioButton_toggled(bool checked);

	void on_deletedGroupBox_toggled(bool arg1);

	void on_newPushButton_clicked();

	void on_tableView_doubleClicked(const QModelIndex &index);

	void on_deletePushButton_clicked();

	void on_refreshPushButton_clicked();

	void on_editPushButton_clicked();

	void on_recoverPushButton_clicked();

	void on_tableView_customContextMenuRequested(const QPoint &pos);

private:
	void createPapersTableContMenu();

private:
	Ui::PapersTableWidget *ui;

	// контекстное меню
	QMenu *papersTableContMenu;
	QAction *newPaperCont;
	QAction *editPaperCont;
	QAction *deletePaperCont;
	QAction *recoverPaperCont;
	//------------------

	TPapersModel* papersProxyModel;
	QSqlTableModel* papersSourceModel;

	FilterPapersTable* papersTableFilter;

	NewPaperDialog *newPaper;
	EditPaperDialog *editPaper;

	// QWidget interface
protected:
	virtual void keyPressEvent(QKeyEvent *event) override;
};

#endif // PAPERSTABLEWIDGET_H
