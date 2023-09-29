#ifndef NEWPAPERTYPEDIALOG_H
#define NEWPAPERTYPEDIALOG_H

#include <QDialog>
#include <QSqlTableModel>
#include "typography.h"

namespace Ui {
	class NewPaperTypeDialog;
}

class NewPaperTypeDialog : public QDialog
{
	Q_OBJECT

public:
	explicit NewPaperTypeDialog(QSqlTableModel *model, QWidget *parent = nullptr);
	~NewPaperTypeDialog();

signals:
	void resetComboBox();

private slots:
	void on_cancelPushButton_clicked();

	void on_okPushButton_clicked();

private:
	Ui::NewPaperTypeDialog *ui;

	QSqlTableModel *paperTypesModel;
};

#endif // NEWPAPERTYPEDIALOG_H
