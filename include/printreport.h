#ifndef PRINTREPORT_H
#define PRINTREPORT_H

#include <QWidget>
#include <QPrinter>
#include <QPaintDevice>
#include <QPainter>
#include <QDebug>
#include <QSqlRecord>
#include "typography.h"

class PrintReport
{

public:
    PrintReport(QSqlRecord record);
	~PrintReport();

	void print();

	void fillReportForm(QString &report);

private:
	QSqlRecord record;

};

#endif // PRINTREPORT_H
