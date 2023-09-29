#ifndef CLIENTDIALOG_H
#define CLIENTDIALOG_H


#include <QDialog>
#include <QSqlTableModel>
#include "typography.h"
#include <QDataWidgetMapper>

extern QSqlDatabase db;


namespace Ui {
class ClientDialog;
}

class ClientDialog : public QDialog
{
    Q_OBJECT

public:
    QDataWidgetMapper* clientMapper;

	explicit ClientDialog(QSqlTableModel *clientModel, int curId, bool isNew, QWidget *parent = nullptr);

    ~ClientDialog();

signals:
	void refreshClientSelectDialog(int curId);

private slots:
    void on_saveNewButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::ClientDialog *ui;
	QSqlTableModel *model;
	int currentId;
	bool isNew;

	// QWidget interface
protected:
	virtual void keyPressEvent(QKeyEvent *event) override;
};

#endif // CLIENTDIALOG_H
