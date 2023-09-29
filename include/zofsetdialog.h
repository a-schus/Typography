#ifndef ZOFSETDIALOG_H
#define ZOFSETDIALOG_H

#include <QDialog>
#include <QSqlRecord>
#include <QSqlTableModel>
#include "typography.h"
#include "clientdialog.h"
#include "clientselectdialog.h"
#include "postpresstablemodel.h"
#include "tlib.h"
#include "tuser.h"

extern QSqlDatabase db;
extern TUser user;

namespace Ui {
class ZOfsetDialog;
}

class ZOfsetDialog : public QDialog
{
    Q_OBJECT

public:
	explicit ZOfsetDialog(QString pm, int pre, QWidget *parent = nullptr);
	explicit ZOfsetDialog(const QSqlRecord zakaz, QString pm, int pre, int copyOrEdit, QWidget *parent = nullptr);

    ~ZOfsetDialog();

private:
	void xeroxUi();
	void initStartParameters(settings::WorkTypes wt);

signals:
    void refreshPasportTable();
    void refreshPasportTableAfterEdit(int);
	void refreshAll();

public slots:
    void setClientId(int id, QString name);

    void refreshSheetsCount();

private slots:

    void on_instanceWdth_valueChanged(int arg1);

    void on_instanceHght_valueChanged(int arg1);

    void on_trimVal_valueChanged(int arg1);

    void on_pushButton_clicked();

    void on_paperType_currentIndexChanged(int index);

    void on_instanceSizeList_currentIndexChanged(int index);

    void on_paperFilter_textEdited(const QString &arg1);

    void on_clientSelect_clicked();

    void on_cmykFaceSpinBox_valueChanged(int arg1);

    void on_pantoneFaceSpinBox_valueChanged(int arg1);

    void on_cmykBackSpinBox_valueChanged(int arg1);

    void on_pantoneBackSpinBox_valueChanged(int arg1);

    void on_ofsLakFaceCheckBox_stateChanged(int arg1);

    void on_ofsLakBackCheckBox_stateChanged(int arg1);

    void on_instCountSpinBox_valueChanged(int arg1);

    void on_issueDateEdit_userDateChanged(const QDate &date);

    void on_addPostpressButton_clicked();

    void on_removePostpressButton_clicked();

    void on_postpressComboBox_currentIndexChanged(int index);

    void on_postpressSheetComboBox_currentIndexChanged(int index);

    void on_addPostpressSheetButton_clicked();

    void on_removePostpressSheetButton_clicked();

    void on_postpressCirculationComboBox_currentIndexChanged(int index);

    void on_addPostpressCirculationButton_clicked();

    void on_removeCirculationPostpressButton_clicked();

    void on_infoTextEdit_textChanged();

    void on_putToDBButton_clicked();

    void on_reSaveButton_clicked();

    void on_productionlineEdit_textChanged(const QString &arg1);

    void on_pantoneListLineEdit_textChanged(const QString &arg1);

	void on_savePreButton_clicked();

	void on_calculatePushButton_clicked();

	void on_paperPriceSpinBox_valueChanged(double arg1);

	void costRefresh();

private:
    Ui::ZOfsetDialog *ui;

    PasportOfset *zakaz_ofs;
    ClientDialog *newClient;
    ClientSelectDialog *selectClient;

    QSqlTableModel *paperTypeList;
    QSqlTableModel *printSizeList;
    QSqlQueryModel postpressList;
    QSqlTableModel *postpressSheetList;
    QSqlQueryModel postpressCirculationList;

    PostpressTableModel *postpressModel;
    PostpressTableModel *postpressSheetModel;
    PostpressTableModel *postpressCirculationModel;

    QString tableName;

    QSqlRecord record;

	int copyOrEdit = -1;
	int preOrNew = 0;

	double (PasportBase::*calculateProductionPrice) ();


public:
	void postpressParsing(const QString s,
						  PostpressTableModel *model,
						  const QSqlQueryModel &list,
						  std::vector <PasportBase::Postpress> &setTo);

};

namespace ZakazName
{
const QString Heidelberg = "Heidelberg";
const QString Xerox = "Xerox";
}

namespace EditOrCopy
{
const int edit = 0;
const int copy = 1;
}
#endif // ZOFSETDIALOG_H
