#include "include/tuser.h"

TUser::TUser()
{

}

bool TUser::hasRightCreateUsers()
{
	QSqlQuery query(db);
	query.prepare("SELECT create_users FROM users WHERE full_name = '" +
				  name.fullName +"'");
	query.exec();
	query.first();
	return query.value(0).toBool();
}

bool TUser::hasDeletePaper()
{
	QSqlQuery query(db);
	query.prepare("SELECT delete_paper FROM users WHERE full_name = '" +
				  name.fullName +"'");
	query.exec();
	query.first();
	return query.value(0).toBool();

}

void TUser::setRightCreateUsers(bool right)
{
	QSqlQuery query(db);
	query.prepare("UPDATE users SET create_users = "
				  + QVariant(right).toString()
				  + " WHERE full_name = '" +
				  name.fullName +"'");
	query.exec();
}

void TUser::setNewPassword(QString fullName, QString pass)
{
	pass = QString(QCryptographicHash::hash(pass.toUtf8(),
											   QCryptographicHash::Sha256).toHex());

	QSqlQuery query(db);
	query.prepare("UPDATE users SET password = '"
				  + pass
				  + "' WHERE full_name = '" +
				  fullName +"'");
	query.exec();
}

QString TUser::password(QString fullName)
{
	QSqlQuery query(db);
	query.prepare("SELECT password FROM users WHERE full_name = '"
				  + fullName + "'");
	query.exec();
	query.first();
	return query.value(0).toString();
}
