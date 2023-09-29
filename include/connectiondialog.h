#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>
#include <QLabel>
#include "typography.h"

extern QSqlDatabase db;

namespace Ui {
class ConnectionDialog;
}

class ConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionDialog(QWidget *parent = nullptr);
    ~ConnectionDialog();

signals:
    void connectMsg(QString);

private slots:
    void on_cancelButton_clicked();

    void on_okButton_clicked();

private:
    Ui::ConnectionDialog *ui;
    QLabel *statusBarMsg;
};

#endif // CONNECTIONDIALOG_H
