#ifndef CLIENTSELECTDIALOG_H
#define CLIENTSELECTDIALOG_H

#include <QDialog>
#include <QSqlTableModel>
#include <QtAlgorithms>
#include "typography.h"
#include <QSqlQuery>
#include "clientdialog.h"

extern QSqlDatabase db;

namespace Ui {
class ClientSelectDialog;
}

class ClientSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientSelectDialog(QWidget *parent = nullptr, int curId = 0);
    ~ClientSelectDialog();

    Ui::ClientSelectDialog *ui;

public slots:
    virtual void on_tableView_doubleClicked(const QModelIndex &index);

    void on_editClientButton_clicked();

signals:
    void sendClientId(int id, QString name);

private slots:
    void on_clientsFilter_textEdited(const QString &arg1);

	virtual void refreshTable(int curId);

    void on_okButton_clicked();

    virtual void on_cancelButton_clicked();

    void on_newClientButton_clicked();

    void on_delClientButton_clicked();

    void on_refreshButton_clicked();


private:
    ClientDialog *newClient;

    QSqlTableModel *clients;
};

// -------------------------------------------------------
// Класс для самостоятельного редактирования базы клиентов

class ClientsTable : public ClientSelectDialog
{

    Q_OBJECT

public:
    explicit ClientsTable(QWidget *parent = nullptr);

    ~ClientsTable();

public slots:
    virtual void on_tableView_doubleClicked(const QModelIndex &index);

	// QWidget interface
protected:
	virtual void keyPressEvent(QKeyEvent *event) override;
};

#endif // CLIENTSELECTDIALOG_H
