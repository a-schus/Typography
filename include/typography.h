#ifndef TYPOGRAPHY_H_
#define TYPOGRAPHY_H_

#include <QTextStream>
#include <string>
#include <vector>
#include <QVector>
#include <QString>
#include <QSqlTableModel>
#include <QVariant>
#include <QDate>
#include <QSqlQuery>
#include "tuser.h"

extern QSqlDatabase db;
extern TUser user;



// Учетные данные
namespace tDBInfo
{
    const QString hostName = "127.0.0.1";
	const QString dbName = "Typography";
	const QString userName = "postgres";
	const QString password = "19schus78";
}

// Таблица PaperTypes
namespace tPaperTypes
{
	const char type = 0;
	const char coefficient = 1;
}

// Таблица Papers
namespace tPapers
{
	const char paperType = 0;
	const char density = 1;
	const char print = 2;
	const char paperName = 3;
	const char weight = 4;
	const char x = 5;
	const char y = 6;
	const char coefficient = 7;
	const char id = 8;
	const char price = 9;
	const char deleted = 10;
}

// Таблица PrintSize
namespace tPrintSize
{
	const char name = 0;
	const char x = 1;
	const char y = 2;
}

// Таблица Prints
namespace tPrints
{
	const char print = 0;
	const QString ofset = "print = 'ofset'";
	const QString xerox = "print = 'xerox'";
}

// Таблица clients
namespace tClients
{
	const char id = 0;
	const char company = 1;
	const char person = 2;
	const char e_mail = 3;
	const char address = 4;
	const char inn = 5;
	const char type = 6;
	const char phone = 7;
	const char discount = 8;
	const char credit = 9;
	const char duty = 10;
	const char companyPhone = 11;
}

// Список возможных постпечатных работ
namespace tPostpress
{
	const char id = 0;
	const char name = 1;
	const char price = 2;
	const char grp = 3;         // номер группы в списке
	const char type_ = 4;
}

// Список печатных машин и видов работ
namespace tWorkTypes
{
	const char id = 0;
	const char name = 1;
	const char sectionCount = 2;
	const char fitting = 3;
	const char maxPaperWidth = 4;
	const char maxPaperHeight = 5;
	const char techFieldWdth = 6;
	const char techFieldHght = 7;
	const char tableName = 8;
	const char type = 9;
	const char fittingTime = 10;
	const char printSpeed = 11;
	const char referens = 12;
}

// Паспорта-заказы на офсет
namespace tPasportOfset
{
	const char id = 0;
	const char client = 1;
	const char issueDate = 2;
	const char production = 3;
	const char paper = 4;
	const char instWdth = 5;
	const char instHght = 6;
	const char trimVal = 7;
	const char faceCMYK = 8;
	const char faceOfsLak = 9;
	const char facePantone = 10;
	const char backCMYK = 11;
	const char backOfsLak = 12;
	const char backPantone = 13;
	const char pantoneList = 14;
	const char instCount = 15;
	const char additionInfo = 16;
	const char postpress = 17;
	const char postpressSheet = 18;
	const char postpressCirculation = 19;
	const char clientName = 20;
	const char workType = 21;
	const char creationDate = 22;
	const char deleted = 23;
	const char printDate = 24;
	const char paid = 25; // заказ оплачен
	const char printed = 26; // заказ отпечатан
	const char prepared = 27; // заказ готов
	const char issued = 28; // заказ выдан
	const char paperPrice = 29;
	const char cost = 30;
	const char owner = 31; // кто создал заказ
}

// Расходные материалы
namespace tSupplies
{
	const char name = 0;
	const char val = 1;
	const char id = 2;
}

namespace tUsers
{
	const char id = 0;
	const char full_name = 1;
	const char print_name = 2;
	const char password = 3;
	const char createUsers = 4; // может ли создавать пользователей
	const char deletePaper = 5;
}

namespace settings
{
	struct WorkTypes
	{
		int id;
		QString name;
		QString type;
		QString tableName;
		int sectionCount;
		int fitting;
		int maxPaperWidth;
		int maxPaperHeight;
		int techFieldWdth;
		int techFieldHght;
		double fittingTime;
		int printSpeed;
	};
	const QString heidelberhWorkType = "Heidelberg";
	const QString xeroxWorkType = "Xerox";
}

namespace newOrPre
{
	const char newPasport = 0;
	const char prePasport = 1;
}


class PasportBase
{
protected:
	//номер заказа
	long long id;

    // заказчик
    struct Client
    {
        int id = -1;						// ID клиента в спарвочнике
        QString company;                    // название организации
        QString person;					// контактное лицо
        std::string address;				// юридический адрес организации

        struct phone						//
        {									//
            std::string description;		// номера телефонов с описанием
            std::string number;				// (например: мобильный, рабочий)
        };									//
        std::vector<phone> phones;			//

        std::string inn;					// ИНН организации
        enum type {client,
                   client_provider,
                   provider};               // тип клиента (клиент, клиент-поставщик, поставщик)
        double discount;					// размер скидки для клиента
    };
    Client client;

    settings::WorkTypes printingMachine;


    QString production;					// наименование продукции
    int fittingCount;
    int sheetsCount;						// чистый тираж в листах
    int overallSheetsCount;               // тираж с приладкой в листах
    long long instancesCount;					// тираж в экземплярах

    // размер экземпляра
    struct InstanceSize
    {
        int width;
        int height;
        int trimVal;
        int preTrimWdth;
        int preTrimHght;
    };
    InstanceSize instanceSize;

    // размер бумаги
    struct Stock_Size
    {
        int id;								// ID материала в справочнике
        int width;
        int height;

        // запечатываемое поле
        int printAreaWidth;
        int printAreaHeight;

    };
    Stock_Size stockSize;

    // цветность печати
    struct Color_Count
    {
        int cmykFace;
        int pantoneFace;
        int ofsLakFace;
        int ofsLakBack;
        int cmykBack;
        int pantoneBack;

        int face;
        int back;
        int runs;							// число прогонов

        QString pantonList;
    };
    Color_Count colorCount;

    int instancesPerSheet;				// колличество экземпляров на листе



	double paperPrice;                      // стоимость одного листа бумаги

	double platePrice;

	double paintWeightPerSheet;

	double paintPricePerSheet;

	double pantonPricePerSheet;

	double ofsLakPricePerSheet;

	double printTime;

	double printHourPriceOfset;

	double printPriceXerox;

public:
	double productionPrice;				// итоговая стоимость заказа




protected:
	QDate creationDate;                 // дата создания заказа

    QDate issueDate;                       // Планируемая дата выдачи продукции

    QString additionInfo;                  // Дополнительные сведения к заказу

	bool deleted = false;						// удален заказ или нет

//-------------------------------------------------------------------------------
// Объявление методов
//-------------------------------------------------------------------------------
    void instPerSheets();                               // вычисление количества экземпляров на листе

    void setInstPreTrim();                           // установить дообрезной размер

    void setFitting();

    void setSheetsCount();

    void setOverallSheetsCount()                     //вычисление общешл тиража в листах
    {overallSheetsCount =
                sheetsCount +
                fittingCount;}


public:
    struct Postpress
    {
        int id;
        QString name;
        double price;
        int count;
        double cost;

		void setId(int id, const QSqlQueryModel &model);
        void setCount(int cnt);
		void setPrice(double price);
    };

    std::vector <Postpress> postpress;
    std::vector <Postpress> postpressSheet;
    std::vector <Postpress> postpressCirculation;


//-------------------------------------------------------------------------------
// Объявление методов
//-------------------------------------------------------------------------------
    PasportBase();

    void setInstCount(long long count);                 // ввод тиража в экземплярах

    void setStockSize(int x = 0, int y = 0);            // ввод размера бумаги
    void setPrintingMachine (settings::WorkTypes &pm)
    {printingMachine = pm;}

    void setStockId(int id)
    {stockSize.id = id;}                               // устанавливает ID бумаги

    void setInstSize(int x, int y);                   // ввод размеров экземпляра

    void setColorCount(int cmykFace,
                       int pantoneFace,
                       int ofsLakFace,
                       int cmykBack,
                       int pantoneBack,
                       int ofsLakBack);  // устанавливает цветность тиража

    void setCmykFace(int cmykFace)
    {colorCount.cmykFace = cmykFace;};

    void setPantoneFace(int pantoneFace)
    {colorCount.pantoneFace = pantoneFace;};

    void setOfslakFace(int ofsLakFace)
    {colorCount.ofsLakFace = ofsLakFace;};

    void setCmykBack(int cmykBack)
    {colorCount.cmykBack = cmykBack;};

    void setPantoneBack(int pantoneBack)
    {colorCount.pantoneBack = pantoneBack;};

    void setOfslakBack(int ofsLakBack)
    {colorCount.ofsLakBack = ofsLakBack;};

    void setPantoneList(QString pantoneList)
    {colorCount.pantonList = pantoneList;}

    void setTrimVal(int trim);                        // установить размер обрезного поля

    void setPaperPrice(double price)                  // установить стоимость листа бумаги
    {paperPrice = price;}

    void setClientId(int id)                          // установить ID клиента
    {client.id = id;}

    void setCreationDate(QDate date)
    {creationDate = date;}

    void setIssueDate(QDate date)
    {issueDate = date;}

    void setProduction(QString prod)
    {production = prod;}

    void setAdditionInfo(QString info)
    {additionInfo = info;}

    void setClientName(QString name)
    {client.person = name;}

    settings::WorkTypes getPrintingMachine()
    {return printingMachine;}

    int getInstPerSheet()                             // возвращает значение переменной instances_per_sheets
    {return instancesPerSheet;};

    int getMaxPaperWidth()                            // получить макс. ширину листа
    {return printingMachine.maxPaperWidth;};

    int getMaxPaperHeight()                            // получить макс. высоту листа
    {return printingMachine.maxPaperHeight;};

    int getStockId()
    {return stockSize.id;}

    int getPrintAreaWidth()                           // получить ширину запечатываемого поля
    {return stockSize.printAreaWidth;};

    int getPrintAreaHeight()                           // получить высоту запечатываемого поля
    {return stockSize.printAreaHeight;};

    int getTrimVal()                                  // получить размер обрезного поля
    {return instanceSize.trimVal;};

    int getPreTrimWidth ()                            //
    {return instanceSize.preTrimWdth;};          //
                                                        // получить дообрезной размер
    int getPreTrimHeight ()                            //
    {return instanceSize.preTrimHght;};          //

    int getClientId()
    {return client.id;};                            // получить ID клиента

    QDate getCreationDate()
    {return creationDate;}

    QDate getIssueDate()
    {return issueDate;}

    double getPaperPrice()                            // получить стоимость листа бумаги
    {return paperPrice;}

    int getCmykFace()
    {return colorCount.cmykFace;};

    int getPantoneFace()
    {return colorCount.pantoneFace;};

    int getOfslakFace()
    {return colorCount.ofsLakFace;};

    int getCmykBack()
    {return colorCount.cmykBack;};

    int getPantoneBack()
    {return colorCount.pantoneBack;};

    int getOfslakBack()
    {return colorCount.ofsLakBack;};

    int getColorFace()
    {return colorCount.face;};

    int getColorBack()
    {return colorCount.back;};

    int getRuns()
    {return colorCount.runs;};

    QString getPantoneList()
    {return colorCount.pantonList;};

    int getSheetsCount()
    {return sheetsCount;}

    int getFittingCount()
    {return fittingCount;}

    int getOverallSheetsCount()
    {return overallSheetsCount;}

    QString getProduction()
    {return production;}

    QString getAdditionInfo()
    {return additionInfo;}

    QString getClientName()
    {return client.person;}

    settings::WorkTypes getWorkType(const QString name, QObject *parent/*, const QSqlDatabase &db*/);

	void setSupplies();

	double calculateOfsetProductionPrice();

	double calculateXeroxProductionPrice();

	friend class PostpressTableModel;
};

class PasportOfset : public PasportBase
{
private:

public:
//-------------------------------------------------------------------------------
// Объявление методов
//-------------------------------------------------------------------------------
    PasportOfset();
    PasportOfset(settings::WorkTypes pm);

	bool putToDb(const QSqlDatabase &db, QString tableName, QString workType, char pasType); // поместить заказ в базу данных

    bool updateToDb(const QSqlDatabase &db, QString tableName, int id);

    bool isReadyToSave();

	long long newPasportId(const QSqlDatabase &db, const QString tableName);

	long long prePasportId(const QSqlDatabase &db, const QString tableName);

	void conformPostpressString(const std::vector <PasportBase::Postpress>& postpressVector, QString& postpressString);
};


QVector <QString> parsingFilterString(const QString& s); // разбивает строку фильтра на подстроки

long long maxId(QSqlTableModel *model, int column = 0);

long long minId(QSqlTableModel *model, int column = 0);

long long curRow(int id, QSqlTableModel *model, int column = 0);

settings::WorkTypes getPrintParams(const QString print);

#endif
