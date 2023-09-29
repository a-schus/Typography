#include "include/mainwindow.h"
#include <QApplication>
//#include "clientselectdialog.h"
#include "include/tuser.h"

QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
TUser user;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    return a.exec();

}
