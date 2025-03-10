#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include "point.h"

void dbCreateConnection(QSqlDatabase db, const QString &connectionName);
void dbAddUser(QSqlDatabase db, QString uid, QString password);
bool dbCheckUser(QSqlDatabase db, QString uid, QString password);
bool dbCheckSignKey(QSqlDatabase db, QString uid);
void dbSetSignKey(QSqlDatabase db, QString uid, QString du, Point Du, QString ds, Point Ds, Point Ppub, QStringList ecparam);
void dbSetEncryptKey(QSqlDatabase db, QString uid, QString touid, QString p, QString s, QString a1, QString a2, QString x1, QString y1, QString x2, QString y2, QString x3, QString y3, QString x4, QString y4);
QString dbGetServerPrivateKey(QSqlDatabase db, QString uid);
QStringList dbGetClientEncryptKey(QSqlDatabase db, QString uid, QString touid);
QStringList dbGetServerEncryptKey(QSqlDatabase db, QString uid, QString touid);
QStringList dbGetEllipticCurve(QSqlDatabase db, QString uid);
QStringList dbSearchSignKey(QSqlDatabase db, QString uid);
QStringList dbSearchEncryptKey(QSqlDatabase db, QString uid, QString touid);

#endif // DATABASE_H
