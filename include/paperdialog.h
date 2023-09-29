#ifndef PAPERDIALOG_H
#define PAPERDIALOG_H

#include <QDataWidgetMapper>
#include <QDialog>
#include <QSqlTableModel>
#include "typography.h"
#include "newpapertypedialog.h"


extern QSqlDatabase db;

namespace Ui {
	class PaperDialog;
}

class PaperDialog : public QDialog
{
	Q_OBJECT

public:
	Ui::PaperDialog *ui;
	QSqlTableModel *model;

	QDataWidgetMapper* papersMapper;

    PaperDialog(QSqlTableModel *papersModel, QWidget *parent = nullptr);
	~PaperDialog();

private slots:
	void on_printComboBox_currentIndexChanged(int index);

	void on_toolButton_clicked();

public slots:
	virtual void on_okPushButton_clicked() = 0;

	virtual void on_cancelPushButton_clicked() = 0;

    /*virtual*/ void resetPaperTypesComboBox();

signals:
	void refreshPaperTable(int focusId);

protected:
	QSqlTableModel *paperTypeModel;
	QSqlTableModel *printModel;
	int currentId;
	bool isNew;
};



// Класс для создания новой бумаги
class NewPaperDialog : public PaperDialog
{
	Q_OBJECT

public:
	NewPaperDialog(QSqlTableModel *papersModel, int curId, QWidget *parent = nullptr);
	~NewPaperDialog();
	void newPaperPrep();
private:
	int currentId;

	// PaperDialog interface
public slots:
	virtual void on_okPushButton_clicked() override;
	virtual void on_cancelPushButton_clicked() override;
};

// Класс для редактирования бумаги
class EditPaperDialog : public PaperDialog
{
	Q_OBJECT

public:
	EditPaperDialog(QSqlTableModel *papersModel, int curId, QWidget *parent = nullptr);
	~EditPaperDialog();

private:
	int currentId;

	// PaperDialog interface
public slots:
	virtual void on_okPushButton_clicked() override;
	virtual void on_cancelPushButton_clicked() override;
};

#endif // PAPERDIALOG_H
