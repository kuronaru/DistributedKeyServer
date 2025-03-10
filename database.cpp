#include "database.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>

void dbCreateConnection(QSqlDatabase db, const QString &connectionName){
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("Lujj474663");
    db.setPort(3306);
    db.setDatabaseName(connectionName);
    if(db.open()) {
        printf("Database %s connected.\n", connectionName.toStdString().c_str());
    }else{
        printf("Database %s connection failed.\n", connectionName.toStdString().c_str());
    }
}

void dbAddUser(QSqlDatabase db, QString uid, QString password){
    printf("dbAddUser: %s\n", uid.toStdString().c_str());
    QSqlQuery query(db);
    query.prepare("INSERT INTO account VALUES(:uid, :password)");
    query.bindValue(":uid", uid);// QString -> QVariant 隐式转换，需要<QVariant>头文件
    query.bindValue(":password", password);
    if(query.exec()){
        printf("SQL executed: %s\n", query.executedQuery().toStdString().c_str());
    }else{
        printf("SQL failed: %s\n", query.executedQuery().toStdString().c_str());
    }
}

bool dbCheckUser(QSqlDatabase db, QString uid, QString password){
    printf("dbCheckUser: %s\n", uid.toStdString().c_str());
    QSqlQuery query(db);
    query.prepare("SELECT * FROM account WHERE uid = :uid");
    query.bindValue(":uid", uid);
    if(query.exec()){
        printf("SQL executed: %s\n", query.executedQuery().toStdString().c_str());
    }else{
        printf("SQL failed: %s\n", query.executedQuery().toStdString().c_str());
    }
    if(query.next()){// 若账号已存在
        printf("Account found.\n");
        if(password.compare(query.value(1).toString()) == 0) return true;
        else{
            printf("Wrong password.\n");
            return false;
        }
    }else{// 若账号不存在
        printf("Account doesn't exist.\n");
//        dbAddUser(db, uid, password);
        return false;
    }
}

bool dbCheckSignKey(QSqlDatabase db, QString uid){
    printf("dbCheckSignKey: %s\n", uid.toStdString().c_str());
    // 检查密钥是否存在或者需要更新
    QSqlQuery query(db);
    query.prepare("SELECT * FROM co_signature WHERE uid = :uid");
    query.bindValue(":uid", uid);
    if(query.exec()){
        printf("SQL executed: %s\n", query.executedQuery().toStdString().c_str());
    }else{
        printf("SQL failed: %s\n", query.executedQuery().toStdString().c_str());
    }
    if(query.next()){
        QString time_str = query.value(9).toString();
        QDateTime begintime = QDateTime::fromString(time_str, "yyyy-MM-dd hh:mm:ss");
        QDateTime curtime = QDateTime::currentDateTime();
        int diff_day = begintime.secsTo(curtime)/86400;// 计算天数差
        if(diff_day >= 7) return false;// 签名密钥更新周期：7天（暂定）
        return true;
    }
    return false;
}

void dbSetSignKey(QSqlDatabase db, QString uid, QString du, Point Du, QString ds, Point Ds, Point Ppub, QStringList ecparam){
    printf("dbSetSignKey: %s\n", uid.toStdString().c_str());
    QSqlQuery query(db);
    query.prepare("SELECT * FROM co_signature WHERE uid = :uid");
    query.bindValue(":uid", uid);
    if(query.exec()){
        printf("SQL executed: %s\n", query.executedQuery().toStdString().c_str());
    }else{
        printf("SQL failed: %s\n", query.executedQuery().toStdString().c_str());
    }
    QDateTime time = QDateTime::currentDateTime();
    QString time_str = time.toString("yyyy-MM-dd hh:mm:ss");
    if(query.next()){
        // update
        query.prepare("UPDATE co_signature "
                      "SET uid = :uid, public_key_x = :px, public_key_y = :py, client_private_key = :du, "
                      "client_public_key_x = :Dux, client_public_key_y = :Duy, server_private_key = :ds, "
                      "server_public_key_x = :Dsx, server_public_key_y = :Dsy, ecparam_a = :a, ecparam_b = :b, "
                      "ecparam_p = :p, ecparam_gx = :gx, ecparam_gy = :gy, update_time = :time "
                      "WHERE uid = :uid");
    }else{
        // insert
        query.prepare("INSERT INTO co_signature "
                      "VALUES(:uid, :px, :py, :du, :Dux, :Duy, :ds, :Dsx, :Dsy, :a, :b, :p, :gx, :gy, :time)");
    }
    query.bindValue(":uid", uid);
    query.bindValue(":px", QString::number(Ppub.x));
    query.bindValue(":py", QString::number(Ppub.y));
    query.bindValue(":du", du);
    query.bindValue(":Dux", QString::number(Du.x));
    query.bindValue(":Duy", QString::number(Du.y));
    query.bindValue(":ds", ds);
    query.bindValue(":Dsx", QString::number(Ds.x));
    query.bindValue(":Dsy", QString::number(Ds.y));
    query.bindValue(":a", ecparam.at(0));
    query.bindValue(":b", ecparam.at(1));
    query.bindValue(":p", ecparam.at(2));
    query.bindValue(":gx", ecparam.at(3));
    query.bindValue(":gy", ecparam.at(4));
    query.bindValue(":time", time_str);
    if(query.exec()){
        printf("SQL executed: %s\n", query.executedQuery().toStdString().c_str());
    }else{
        printf("SQL failed: %s\n", query.executedQuery().toStdString().c_str());
    }
}

void dbSetEncryptKey(QSqlDatabase db, QString uid, QString touid, QString p, QString s, QString a1, QString a2,
                     QString x1, QString y1, QString x2, QString y2, QString x3, QString y3, QString x4, QString y4){
    QSqlQuery query(db);
    query.prepare("SELECT * FROM sm4encrypt WHERE uid = :uid AND to_uid = :touid");
    query.bindValue(":uid", uid);
    query.bindValue(":touid", touid);
    if(query.exec()){
        printf("SQL executed: %s\n", query.executedQuery().toStdString().c_str());
    }else{
        printf("SQL failed: %s\n", query.executedQuery().toStdString().c_str());
    }
    QDateTime time = QDateTime::currentDateTime();
    QString time_str = time.toString("yyyy-MM-dd hh:mm:ss");
    if(query.next()){
        // update
        query.prepare("UPDATE sm4encrypt "
                      "SET mod_p = :p, polynomial_a1 = :a1, polynomial_a2 = :a2, sm4_private_key = :s, "
                      "client1_private_key_x = :x1, client1_private_key_y = :y1, client2_private_key_x = :x2, "
                      "client2_private_key_y = :y2, server_private_key1_x = :x3, server_private_key1_y = :y3, "
                      "server_private_key2_x = :x4, server_private_key2_y = :y4, update_time = :time "
                      "WHERE uid = :uid AND to_uid = :touid");
    }else{
        // insert
        query.prepare("INSERT INTO sm4encrypt "
                      "VALUES(:uid, :touid, :p, :a1, :a2, :s, :x1, :y1, :x2, :y2, :x3, :y3, :x4, :y4, :time)");
    }
    query.bindValue(":uid", uid);
    query.bindValue(":touid", touid);
    query.bindValue(":p", p);
    query.bindValue(":s", s);
    query.bindValue(":a1", a1);
    query.bindValue(":a2", a2);
    query.bindValue(":x1", x1);
    query.bindValue(":y1", y1);
    query.bindValue(":x2", x2);
    query.bindValue(":y2", y2);
    query.bindValue(":x3", x3);
    query.bindValue(":y3", y3);
    query.bindValue(":x4", x4);
    query.bindValue(":y4", y4);
    query.bindValue(":time", time_str);
    if(query.exec()){
        printf("SQL executed: %s\n", query.executedQuery().toStdString().c_str());
    }else{
        printf("SQL failed: %s\n", query.executedQuery().toStdString().c_str());
    }
    // 同时更新用户互换的条目
    query.bindValue(":uid", touid);
    query.bindValue(":touid", uid);
    query.bindValue(":x1", x2);
    query.bindValue(":y1", y2);
    query.bindValue(":x2", x1);
    query.bindValue(":y2", y1);
    if(query.exec()){
        printf("SQL executed: %s\n", query.executedQuery().toStdString().c_str());
    }else{
        printf("SQL failed: %s\n", query.executedQuery().toStdString().c_str());
    }
}

QString dbGetServerPrivateKey(QSqlDatabase db, QString uid){
    QSqlQuery query(db);
    query.prepare("SELECT * FROM co_signature WHERE uid = :uid");
    query.bindValue(":uid", uid);
    if(query.exec()){
        printf("SQL executed: %s\n", query.executedQuery().toStdString().c_str());
    }else{
        printf("SQL failed: %s\n", query.executedQuery().toStdString().c_str());
    }
    QString ds;
    if(query.next()){
        ds = query.value(6).toString();
    }
    return ds;
}

QStringList dbGetClientEncryptKey(QSqlDatabase db, QString uid, QString touid){
    QSqlQuery query(db);
    query.prepare("SELECT * FROM sm4encrypt WHERE uid = :uid AND to_uid = :touid");
    query.bindValue(":uid", uid);
    query.bindValue(":touid", touid);
    if(query.exec()){
        printf("SQL executed: %s\n", query.executedQuery().toStdString().c_str());
    }else{
        printf("SQL failed: %s\n", query.executedQuery().toStdString().c_str());
    }
    QStringList data;
    if(query.next()){
        data << query.value(2).toString() << query.value(6).toString() << query.value(7).toString() << query.value(14).toString();
    }
    return data;
}

QStringList dbGetServerEncryptKey(QSqlDatabase db, QString uid, QString touid){
    QSqlQuery query(db);
    query.prepare("SELECT * FROM sm4encrypt WHERE uid = :uid AND to_uid = :touid");
    query.bindValue(":uid", uid);
    query.bindValue(":touid", touid);
    if(query.exec()){
        printf("SQL executed: %s\n", query.executedQuery().toStdString().c_str());
    }else{
        printf("SQL failed: %s\n", query.executedQuery().toStdString().c_str());
    }
    QStringList data;
    if(query.next()){
        data << query.value(10).toString() << query.value(11).toString() << query.value(12).toString() << query.value(13).toString();
    }
    return data;
}

QStringList dbGetEllipticCurve(QSqlDatabase db, QString uid){
    QSqlQuery query(db);
    query.prepare("SELECT * FROM co_signature WHERE uid = :uid");
    query.bindValue(":uid", uid);
    if(query.exec()){
        printf("SQL executed: %s\n", query.executedQuery().toStdString().c_str());
    }else{
        printf("SQL failed: %s\n", query.executedQuery().toStdString().c_str());
    }
    QStringList data;
    if(query.next()){
        data << query.value(9).toString() << query.value(10).toString() << query.value(11).toString()
             << query.value(12).toString() << query.value(13).toString();
    }
    return data;
}

QStringList dbSearchSignKey(QSqlDatabase db, QString uid){
    QSqlQuery query(db);
    query.prepare("SELECT * FROM co_signature WHERE uid = :uid");
    query.bindValue(":uid", uid);
    if(query.exec()){
        printf("SQL executed: %s\n", query.executedQuery().toStdString().c_str());
    }else{
        printf("SQL failed: %s\n", query.executedQuery().toStdString().c_str());
    }
    QStringList data;
    if(query.next()){
        data << query.value(1).toString() << query.value(2).toString() << query.value(3).toString()
             << query.value(4).toString() << query.value(5).toString() << query.value(6).toString()
             << query.value(7).toString() << query.value(8).toString() << query.value(9).toString()
             << query.value(10).toString() << query.value(11).toString() << query.value(12).toString()
             << query.value(13).toString() << query.value(14).toDateTime().toString("yyyy-MM-dd hh:mm:ss");
    }
    return data;
}

QStringList dbSearchEncryptKey(QSqlDatabase db, QString uid, QString touid){
    QSqlQuery query(db);
    query.prepare("SELECT * FROM sm4encrypt WHERE uid = :uid AND to_uid = :touid");
    query.bindValue(":uid", uid);
    query.bindValue(":touid", touid);
    if(query.exec()){
        printf("SQL executed: %s\n", query.executedQuery().toStdString().c_str());
    }else{
        printf("SQL failed: %s\n", query.executedQuery().toStdString().c_str());
    }
    QStringList data;
    if(query.next()){
        data << query.value(2).toString() << query.value(3).toString() << query.value(4).toString()
             << query.value(5).toString() << query.value(6).toString() << query.value(7).toString()
             << query.value(8).toString() << query.value(9).toString() << query.value(10).toString()
             << query.value(11).toString() << query.value(12).toString() << query.value(13).toString()
             << query.value(14).toDateTime().toString("yyyy-MM-dd hh:mm:ss");
    }
    return data;
}
