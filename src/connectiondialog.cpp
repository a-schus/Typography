#include "include/connectiondialog.h"
#include "ui_connectiondialog.h"
#include <QSqlError>
#include <QMessageBox>
#include <QObject>


ConnectionDialog::ConnectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionDialog)
{
    ui->setupUi(this);
    ui->ipLineEdit->setText(tDBInfo::hostName);
    ui->nameLineEdit->setText(tDBInfo::userName);
    ui->passLineEdit->setText(tDBInfo::password);
}

ConnectionDialog::~ConnectionDialog()
{
    delete ui;
}

void ConnectionDialog::on_cancelButton_clicked()
{
    this->close();
}


void ConnectionDialog::on_okButton_clicked()
{
    if (db.isOpen())
        db.close();
    db.setHostName(ui->ipLineEdit->text());
    db.setDatabaseName(tDBInfo::dbName);
    db.setUserName(ui->nameLineEdit->text());
    db.setPassword(ui->passLineEdit->text());
    db.open();
    if (!db.open()){
        QMessageBox msg;
        msg.setText("Не удалось подключиться к базе данных!\n"
                    + db.lastError().text());
        msg.exec();
        return;
    }
    this->close();
}

