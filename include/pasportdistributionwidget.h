#ifndef PASPORTDISTRIBUTIONWIDGET_H
#define PASPORTDISTRIBUTIONWIDGET_H

#include <QWidget>
#include "pasportbaseviewwidget.h"

namespace Ui {
	class PasportDistributionWidget;
}

class PasportDistributionWidget : public QWidget
{
	Q_OBJECT

public:
	explicit PasportDistributionWidget(QWidget *parent = nullptr);
	~PasportDistributionWidget();

signals:
	void closeAll();

	void refreshTable();

private slots:
	void on_toolButton_clicked();

	void on_currentDateWidget_clicked(const QDate &date);

	void on_toolButton_2_clicked();

	void on_toolButton_3_clicked();

private:
	Ui::PasportDistributionWidget *ui;

	PasportBaseViewWidget *currentDatePasportTable;
	PasportBaseViewWidget *pasportTable;

	QVBoxLayout *layout_1;
	QVBoxLayout *layout_2;
};

#endif // PASPORTDISTRIBUTIONWIDGET_H
