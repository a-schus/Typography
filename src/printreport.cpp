#include "include/printreport.h"

#include <QFile>
#include <QModelIndex>
#include <QPrintDialog>
#include <QTextDocument>

PrintReport::PrintReport(QSqlRecord record) :
	record(record)
{

}

PrintReport::~PrintReport()
{
}

void PrintReport::print()
{
	QFile report(":/reports/reportOfset.html");
	if (!report.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Файл не открывается!";
		return;
	}
	qDebug() << "Файл открыт!";
	QByteArray reportBytes = report.readAll();
	report.close();
	QString reportLines = QString(reportBytes);
	fillReportForm(reportLines);

	QTextDocument* printReport = new QTextDocument();
	printReport->setHtml(reportLines);
	printReport->setMetaInformation(QTextDocument::MetaInformation::DocumentTitle, reportLines);
	printReport->setDocumentMargin(0);
	printReport->setUseDesignMetrics(true);

	QPrinter printer(QPrinter::ScreenResolution);
	QPrintDialog *dlg = new QPrintDialog(&printer);
	if(!dlg->exec())
		return;
	delete dlg;
	printer.setPageSize(QPageSize::A4);
	printer.setOutputFormat(QPrinter::OutputFormat::NativeFormat);
	printer.setPageMargins(QMarginsF(0, 0, 0, 0), QPageLayout::Millimeter);
	printer.setFullPage(true);

	printReport->setPageSize(QSizeF(printer.pageRect(QPrinter::Unit::DevicePixel).size()));
	qDebug() << printReport->pageSize();

	QPainter painter(&printer);
	printReport->drawContents(&painter);

	delete printReport;
}

void PrintReport::fillReportForm(QString &report)
{
	report.replace("@@id@@", record.value(tPasportOfset::id).toString());
	report.replace("@@НаимнованиеПродукции@@", record.value(tPasportOfset::production).toString());

	return;
}
