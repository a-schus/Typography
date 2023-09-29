#include "include/clientselectdialog.h"
#include "ui_clientselectdialog.h"
#include <QMessageBox>
#include <QSqlError>
#include <QVariant>
#include <QSqlRecord>
#include <include/typography.h>
#include <QModelIndexList>
#include <QKeyEvent>

ClientSelectDialog::ClientSelectDialog(QWidget *parent, int curId) :
    QDialog(parent),
    ui(new Ui::ClientSelectDialog)
{
    ui->setupUi(this);
    // формирование таблицы клиентов
    clients = new QSqlTableModel;
    clients->setTable("clients");
    clients->setEditStrategy(QSqlTableModel::OnRowChange);
    clients->setSort(tClients::person, Qt::SortOrder::AscendingOrder);
    clients->select();

    clients->setHeaderData(tClients::company,
                           Qt::Horizontal,
                           QObject::tr("Компания"));
    clients->setHeaderData(tClients::person,
                           Qt::Horizontal,
                           QObject::tr("Представитель"));
    clients->setHeaderData(tClients::e_mail,
                           Qt::Horizontal,
                           QObject::tr("E-mail"));
    clients->setHeaderData(tClients::phone,
                           Qt::Horizontal,
                           QObject::tr("Телефон"));

    ui->tableView->setModel(clients);
    ui->tableView->hideColumn(0);
    ui->tableView->hideColumn(4);
    ui->tableView->hideColumn(5);
    ui->tableView->hideColumn(6);
    ui->tableView->hideColumn(8);
    ui->tableView->hideColumn(9);
    ui->tableView->hideColumn(10);


    ui->tableView->horizontalHeader()->setSectionResizeMode(tClients::company,
                                                            QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(tClients::person,
                                                            QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(tClients::e_mail,
                                                            QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(tClients::phone,
                                                            QHeaderView::ResizeToContents);

    ui->tableView->selectRow(curRow(curId, clients));
	ui->tableView->setFocus();
}

ClientSelectDialog::~ClientSelectDialog()
{
    delete ui;
    delete clients;

}

void ClientSelectDialog::on_clientsFilter_textEdited(const QString &arg1)
{
    QString where = "LOWER(company) LIKE LOWER('%" + arg1 + "%') OR "
                    "LOWER(person) LIKE LOWER('%" + arg1 + "%')";
    clients->setFilter(where);
    ui->tableView->setModel(clients);
}


void ClientSelectDialog::on_okButton_clicked()
{
    int id;
    QString name;

    if (ui->tableView->currentIndex().row() == -1)
    {
        id = -1;
        name = "";
    }
    else
    {
        QModelIndex idx = clients->index(ui->tableView->currentIndex().row(), 0, QModelIndex());
        id = clients->data(idx).toInt();

        idx = clients->index(ui->tableView->currentIndex().row(), 1, QModelIndex());
        name = clients->data(idx).toString();

        if (name.size())
        {
            idx = clients->index(ui->tableView->currentIndex().row(), 2, QModelIndex());
            name += ", " + clients->data(idx).toString();
        }
        else
        {
            idx = clients->index(ui->tableView->currentIndex().row(), 2, QModelIndex());
            name = clients->data(idx).toString();
        }
    }
    emit sendClientId(id, name);
    this->close();
}

void ClientSelectDialog::on_cancelButton_clicked()
{
	this->close();
}


void ClientSelectDialog::on_newClientButton_clicked()
{
	int currentRow = clients->data(clients->index(ui->tableView->currentIndex().row(),
												  tClients::id)).toInt();
	ui->clientsFilter->setText("");
    on_clientsFilter_textEdited("");
    int lastRow = clients->rowCount(QModelIndex());
    clients->insertRow(lastRow);
    ui->tableView->selectRow(lastRow);

	newClient = new ClientDialog(clients, currentRow, true, this);
    newClient->setAttribute(Qt::WA_DeleteOnClose);
    newClient->setWindowFlag(Qt::WindowMinimizeButtonHint);
    newClient->setWindowModality(Qt::WindowModal);
    newClient->clientMapper->setCurrentModelIndex(ui->tableView->currentIndex());
    newClient->show();

    QObject::connect(newClient, &ClientDialog::refreshClientSelectDialog,
                     this, &ClientSelectDialog::refreshTable);
}

void ClientSelectDialog::refreshTable(int curId)
{
    clients->select();
	ui->tableView->selectRow(curId);
    ui->tableView->setFocus();
}

void ClientSelectDialog::on_tableView_doubleClicked(const QModelIndex &index)
{

    on_okButton_clicked();
}


void ClientSelectDialog::on_editClientButton_clicked()
{
	int currentRow = clients->data(clients->index(ui->tableView->currentIndex().row(),
												  tClients::id)).toInt();
	newClient = new ClientDialog(clients, currentRow, false, this);
    newClient->setAttribute(Qt::WA_DeleteOnClose);
    newClient->setWindowModality(Qt::WindowModal);
    newClient->clientMapper->setCurrentModelIndex(ui->tableView->currentIndex());
    newClient->show();
	QObject::connect(newClient, &ClientDialog::refreshClientSelectDialog,
					 this, &ClientSelectDialog::refreshTable);

    ui->tableView->setFocus();
}

void ClientSelectDialog::on_delClientButton_clicked()
{
    QMessageBox msg;
    msg.setText("Удалить клиента из базы?");
    msg.setWindowTitle("Удаление");
    msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msg.setIcon(QMessageBox::Warning);
    msg.setDefaultButton(QMessageBox::Cancel);
    int chois = msg.exec();
    if (chois == QMessageBox::Cancel)
    {
        ui->tableView->setFocus();
        return;
    }

	QModelIndexList indexList = ui->tableView->selectionModel()->selectedRows();

	// Упорядочиваем шндексы строк по убыванию
	// для последующего коррктного удаления строк из модели
	for (int i = 0; i < indexList.count(); ++i)
		for (int j = 0; j < indexList.count() - 1; ++j)
			if (indexList.at(j).row() < indexList.at(j + 1).row())
				indexList.swapItemsAt(j, j+1);

	foreach (QModelIndex idx, indexList)
	{
		clients->removeRows(idx.row(), 1);
	}

	qDebug() << ui->tableView->currentIndex().row();
    clients->select();
}



ClientsTable::ClientsTable(QWidget *parent) :
    ClientSelectDialog(parent)
{
    this->setWindowFlag(Qt::WindowMinMaxButtonsHint);

    ui->okButton->hide();
    ui->cancelButton->hide();
    this->setWindowTitle("База клиентов");
}

ClientsTable::~ClientsTable()
{
}

void ClientsTable::on_tableView_doubleClicked(const QModelIndex &index)
{
    on_editClientButton_clicked();
    return;
}

void ClientSelectDialog::on_refreshButton_clicked()
{
    int row = ui->tableView->currentIndex().row();
    int id = clients->data(clients->index(row, 0)).toInt();
	clients->select();
    ui->tableView->selectRow(curRow(id, clients));
    ui->tableView->setFocus();
}


void ClientsTable::keyPressEvent(QKeyEvent *event)
{
		if (event->key() == Qt::Key::Key_Escape)
			return;
	QDialog::keyPressEvent(event);
}
