#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QResizeEvent>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QToolButton>
#include "zofsetdialog.h"
#include "typography.h"
#include <QLabel>
#include "clientselectdialog.h"
#include "connectiondialog.h"
#include "pasportbaseviewwidget.h"
#include "printreport.h"
#include <QTextDocument>
#include "paperstablewidget.h"
#include "pasportdistributionwidget.h"
#include "authorization.h"
#include "tuser.h"
#include "changepassworddialog.h"

extern QSqlDatabase db;
extern TUser user;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

signals:
    void closeAll();
    void refreshPasportTable();
	void refreshAll();

public slots:
    void setConnectMsg(QString);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_Quit_triggered();

    void on_clientsTable_triggered();

    void on_connection_triggered();

    void on_newHeidelberg_triggered();

    void on_newXerox_triggered();

    void on_tabWidget_tabCloseRequested(int index);

    void on_newTab_triggered();

	void on_newUser_triggered();

	void on_changePassword_triggered();

private:
    Ui::MainWindow *ui;
    ZOfsetDialog *zOfsWin;
    QLabel *statusBarMsg;
    ClientsTable *clientsTable;
    ClientsTable *clientsTableTab;
    ConnectionDialog *connection;

    QSqlTableModel *pasportOfsetModel;

    QMenu *pasportTableViewContMenu;

public:
	PasportBaseViewWidget *general;
	PasportBaseViewWidget *prePasportTab;
	PasportDistributionWidget *pasportDistributionTab;
	PapersTableWidget* papersTab;
};


#endif // MAINWINDOW_H
