#include "include/mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlError>
#include <QMessageBox>
#include <stdlib.h>
#include <QObject>
#include <QSqlRecord>
#include <QPushButton>
#include <QToolButton>
#include <QPrintDialog>
#include <QString>
#include <QTextEdit>
#include <QFile>
#include <QStringList>
#include <QAbstractTextDocumentLayout>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Подключение к базе данных
    db.setHostName(tDBInfo::hostName);
    db.setDatabaseName(tDBInfo::dbName);
    db.setUserName(tDBInfo::userName);
    db.setPassword(tDBInfo::password);
    db.open();

    if (!db.isOpen())
    {
        connection = new ConnectionDialog(this);
        connection->exec();
        delete connection;
    }

    ui->setupUi(this);


    if (!db.open()){
        abort();
    }

	statusBarMsg = new QLabel;
    statusBarMsg->setText(" Соединения с базой установлено ");
    statusBarMsg->setStyleSheet("QLabel { color : green; }");
    ui->statusbar->addWidget(statusBarMsg);

	// Авторизация
    Authorization* authorization = new Authorization(this);
    authorization->setAttribute(Qt::WA_DeleteOnClose);
    authorization->exec();
    this->setWindowTitle(this->windowTitle() + " | " + user.name.fullName);


    ui->tabWidget->setStyleSheet("QTabBar::close-button "
                                 "{image: url(:/images/close.svg)}");

	// Добавляем вкладку распределения заказов
	pasportDistributionTab = new PasportDistributionWidget(this);
	QObject::connect(this, &MainWindow::closeAll,
					 pasportDistributionTab, &PasportDistributionWidget::closeAll);
	QObject::connect(this, &MainWindow::refreshAll,
					 pasportDistributionTab, &PasportDistributionWidget::refreshTable);

	ui->tabWidget->addTab(pasportDistributionTab, "Распределение заказов");

	// Добавляем вкладку предварительных расчетов
	prePasportTab = new PasportBaseViewWidget(newOrPre::prePasport, this);
	prePasportTab->setAttribute(Qt::WA_DeleteOnClose);

	QObject::connect(this, &MainWindow::closeAll,
					 prePasportTab, &PasportBaseViewWidget::closeAll);
	ui->tabWidget->addTab(prePasportTab, "Предварительные расчеты");

	// Добавляем вкладку каталога бумаги
	papersTab = new PapersTableWidget(ui->tabWidget);
	ui->tabWidget->addTab(papersTab, "Каталог бумаги");

	// Добавляем вкладку таблицы клиентов
	clientsTableTab = new ClientsTable(ui->tabWidget);
    ui->tabWidget->addTab(clientsTableTab, "Клиенты");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete statusBarMsg;
    delete clientsTableTab;
}

void MainWindow::setConnectMsg(QString msg)
{
    statusBarMsg->setText(msg);
    ui->statusbar->addWidget(statusBarMsg);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit closeAll();
	QMainWindow::closeEvent(event);
}

void MainWindow::on_Quit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_clientsTable_triggered()
{
    if (!db.isOpen())
    {
        QMessageBox msg;
        msg.setText("Не удалось подключиться к базе данных!\n"
                    + db.lastError().text());
        msg.exec();
        return;
    }

    clientsTable = new ClientsTable();
    clientsTable->setAttribute(Qt::WA_DeleteOnClose);

    // при закрытии главного окна закрываем все остальные окна
    QObject::connect(this, &MainWindow::closeAll,
                     clientsTable, &ClientSelectDialog::close);

    clientsTable->show();
}


void MainWindow::on_connection_triggered()
{
    connection = new ConnectionDialog(this);

    QObject::connect(connection, &ConnectionDialog::connectMsg,
                     this, &MainWindow::setConnectMsg);

    connection->setAttribute(Qt::WA_DeleteOnClose);
    connection->show();
}

void MainWindow::on_newHeidelberg_triggered()
{
    if (!db.isOpen())
    {
        QMessageBox msg;
        msg.setText("Не удалось подключиться к базе данных!\n"
                    + db.lastError().text());
        msg.exec();
        return;
    }

	// Проверяем какая вкладка основного окна активна, в зависимости от этого
	// формируем диалог создания нового заказа и связываем нужные сигналы и слоты
	PasportBaseViewWidget* pbvw;
	pbvw = qobject_cast<PasportBaseViewWidget*>(ui->tabWidget->currentWidget());
	if (pbvw != nullptr)
	{
		zOfsWin = new ZOfsetDialog(settings::heidelberhWorkType, pbvw->preOrNew);
		QObject::connect(zOfsWin, &ZOfsetDialog::refreshPasportTable,
						 pbvw, &PasportBaseViewWidget::refreshTable);
	}
	else
	{
		zOfsWin = new ZOfsetDialog(settings::heidelberhWorkType, newOrPre::newPasport);
		PasportDistributionWidget* pdw = qobject_cast<PasportDistributionWidget*>(ui->tabWidget->currentWidget());
		if (pdw)
		{
			QObject::connect(zOfsWin, &ZOfsetDialog::refreshPasportTable,
							 pdw, &PasportDistributionWidget::refreshTable);
		}
	}


    QObject::connect(this, &MainWindow::closeAll,
                     zOfsWin, &ZOfsetDialog::close);

    zOfsWin->setAttribute(Qt::WA_DeleteOnClose);
    zOfsWin->show();
}


void MainWindow::on_newXerox_triggered()
{
    if (!db.isOpen())
    {
        QMessageBox msg;
        msg.setText("Не удалось подключиться к базе данных!\n"
                    + db.lastError().text());
        msg.exec();
        return;
    }

    // Проверяем какая вкладка основного окна активна, в зависимости от этого
	// формируем диалог создания нового заказа и связываем нужные сигналы и слоты
	PasportBaseViewWidget* pbvw;
	pbvw = qobject_cast<PasportBaseViewWidget*>(ui->tabWidget->currentWidget());
	if (pbvw != nullptr)
	{
		zOfsWin = new ZOfsetDialog(settings::xeroxWorkType, pbvw->preOrNew);
		QObject::connect(zOfsWin, &ZOfsetDialog::refreshPasportTable,
						 pbvw, &PasportBaseViewWidget::refreshTable);
	}
	else
	{
		zOfsWin = new ZOfsetDialog(settings::xeroxWorkType, newOrPre::newPasport);
		PasportDistributionWidget* pdw = qobject_cast<PasportDistributionWidget*>(ui->tabWidget->currentWidget());
		if (pdw)
		{
			QObject::connect(zOfsWin, &ZOfsetDialog::refreshPasportTable,
							 pdw, &PasportDistributionWidget::refreshTable);
		}
	}

	QObject::connect(this, &MainWindow::closeAll,
					 zOfsWin, &ZOfsetDialog::close);

	zOfsWin->setAttribute(Qt::WA_DeleteOnClose);
	zOfsWin->show();

}


void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if(ui->tabWidget->count() < 2)
        return;
    ui->tabWidget->removeTab(index);
}


void MainWindow::on_newTab_triggered()
{
//    PasportBaseViewWidget *newTab = new PasportBaseViewWidget(ui->tabWidget);
//    newTab->setAttribute(Qt::WA_DeleteOnClose);
//    QObject::connect(this, &MainWindow::closeAll, newTab,
//                     &PasportBaseViewWidget::closeInheritor);
//    ui->tabWidget->addTab(newTab, "Паспорта-заказы");
//    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
	//    newTab->refresh();
}


void MainWindow::on_newUser_triggered()
{
	QSqlTableModel* userModel = new QSqlTableModel(this, db);
	userModel->setTable("users");
	NewUserDialog* newUser = new NewUserDialog(userModel, this);
	newUser->setAttribute(Qt::WA_DeleteOnClose);
	newUser->exec();
	delete  userModel;
}


void MainWindow::on_changePassword_triggered()
{
	changePasswordDialog* changePassword = new changePasswordDialog(this);
    changePassword->setAttribute(Qt::WA_DeleteOnClose);
    changePassword->exec();
}

