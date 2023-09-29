#include "zofsetdialogheidelberg.h"

// -----------------------------------------------------------------------------------------------
// ZOfsetDialogHeidelberg	ГЕЙДЕЛЬ, НОВЫЙ ЗАКАЗ
// -----------------------------------------------------------------------------------------------
ZOfsetDialogHeidelberg::ZOfsetDialogHeidelberg(QString pm, int pre, QWidget *parent) :
	ZOfsetDialog(pm, pre, parent)
{

}

ZOfsetDialogHeidelberg::ZOfsetDialogHeidelberg(const QSqlRecord zakaz, QString pm, int pre, int editOrCopy, QWidget *parent) :
	ZOfsetDialog(zakaz, pm, pre, editOrCopy, parent)
{

}

void ZOfsetDialogHeidelberg::on_calculatePushButton_clicked()
{
	ui->price->setText(QString::number(zakaz_ofs->calculateOfsetProductionPrice()) + " руб.");
}







// -----------------------------------------------------------------------------------------------
// ZOfsetDialogXerox	КСЕРОКС, НОВЫЙ ЗАКАЗ
// -----------------------------------------------------------------------------------------------
ZOfsetDialogXerox::ZOfsetDialogXerox(QString pm, int pre, QWidget *parent) :
	ZOfsetDialog(pm, pre, parent)
{
	xeroxUi();

}

ZOfsetDialogXerox::ZOfsetDialogXerox(const QSqlRecord zakaz, QString pm, int pre, int editOrCopy, QWidget *parent) :
	ZOfsetDialog(zakaz, pm, pre, editOrCopy, parent)
{
	xeroxUi();

}

void ZOfsetDialogXerox::on_calculatePushButton_clicked()
{
	ui->price->setText(QString::number(zakaz_ofs->calculateXeroxProductionPrice()) + " руб.");
}
