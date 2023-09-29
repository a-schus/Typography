#ifndef ZOFSETDIALOGHEIDELBERG_H
#define ZOFSETDIALOGHEIDELBERG_H

#include "zofsetdialog.h"

// -----------------------------------------------------------------------------------------------
// ZOfsetDialogHeidelberg	ГЕЙДЕЛЬ, НОВЫЙ ЗАКАЗ
// -----------------------------------------------------------------------------------------------
class ZOfsetDialogHeidelberg : public ZOfsetDialog
{
	Q_OBJECT

public:
	ZOfsetDialogHeidelberg(QString pm, int pre, QWidget *parent = nullptr);
	ZOfsetDialogHeidelberg(const QSqlRecord zakaz, QString pm, int pre, int editOrCopy, QWidget *parent = nullptr);


	// ZOfsetDialog interface
private slots:
	virtual void on_calculatePushButton_clicked() override;

private:

};






// -----------------------------------------------------------------------------------------------
// ZOfsetDialogXerox	КСЕРОКС, НОВЫЙ ЗАКАЗ
// -----------------------------------------------------------------------------------------------
class ZOfsetDialogXerox : public ZOfsetDialog
{
	Q_OBJECT

public:
	ZOfsetDialogXerox(QString pm, int pre, QWidget *parent = nullptr);
	ZOfsetDialogXerox(const QSqlRecord zakaz, QString pm, int pre, int editOrCopy, QWidget *parent = nullptr);


	// ZOfsetDialog interface
private slots:
	virtual void on_calculatePushButton_clicked() override;

private:

};

#endif // ZOFSETDIALOGHEIDELBERG_H
