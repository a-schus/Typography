#ifndef TUSER_H
#define TUSER_H

#include <QSqlTableModel>
#include <QSqlQuery>
#include <QString>
#include <QSqlError>
#include <QCryptographicHash>

extern QSqlDatabase db;

class TUser
{
public:
	struct Name
	{
		QString fullName;
		QString printName;
	};
	Name name;

	TUser();

    bool hasRightCreateUsers();
	bool hasDeletePaper();
    void setRightCreateUsers(bool right);
	void setNewPassword(QString fullName, QString pass);
	QString password(QString fullName);
};

#endif // TUSER_H
