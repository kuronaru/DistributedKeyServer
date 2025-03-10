#include "serverthread.h"
#include "ellipticcurve.h"
#include "database.h"

ServerThread::ServerThread(qintptr sockDesc, QSqlDatabase db, QObject *parent) : QThread(parent), db(db), m_sockDesc(sockDesc){
}

ServerThread::~ServerThread(){
    m_socket->close();
    delete m_socket;
}

QString ServerThread::getUid(){
    return uid;
}

void ServerThread::run(void){
    // 创建socket
    m_socket = new MySocket(m_sockDesc);
    if(!m_socket->setSocketDescriptor(m_sockDesc)) {
        return;
    }
    // 关联信号槽
    connect(m_socket, &MySocket::disconnected, this, &ServerThread::disconnectToHost);
    connect(m_socket, &MySocket::dataReady, this, &ServerThread::recvData);
    connect(m_socket, &MySocket::sendData_verifyLogin, this, &ServerThread::verifyLogin);
    connect(m_socket, &MySocket::sendData_generateSignKey, this, &ServerThread::generateSignKey);
    connect(m_socket, &MySocket::sendData_generateSignature, this, &ServerThread::generateSignature);
    connect(m_socket, &MySocket::sendData_sendSplitKey, this, &ServerThread::sendSplitKey);
    connect(this, &ServerThread::sendData, m_socket, &MySocket::sendData);
    connect(this, &ServerThread::waitForData, m_socket, &MySocket::recvSyncData);
//    connect(this, &ServerThread::sendMessage, this, &ServerThread::sendMessageSlot);

    this->exec();
}

void ServerThread::verifyLogin(const QByteArray &data){
    // 一次性接收用户名密码字符串，格式："username,password"
    QString tmpdata = data;
    QStringList datalist = tmpdata.split(",");// 处理用户名密码
    uid = datalist.at(0);
    QString password = datalist.at(1);
    emit consoleOutput("收到数据：" + data);
    // 验证登录
    if(dbCheckUser(db, uid, password)){
        emit sendData("success");
        emit consoleOutput(uid + "登录成功");
        emit addUidToMap(this, uid);
        printf("UID: %s logged in.\n", uid.toStdString().c_str());
        // 检查密钥
        if(dbCheckSignKey(db, uid)){
//            emit sendData("loginfinish");
            emit consoleOutput("签名密钥已是最新");
            printf("Sign key is up to date.\n");
        }else{
            emit consoleOutput("签名密钥需要更新");
//            emit sendData("gensignkey");
//            emit waitForData(GENSIGNKEY);
        }
    }else{
        emit sendData("fail");
        emit consoleOutput(uid + "登陆失败");
        printf("UID: %s login failed.\n", uid.toStdString().c_str());
    }
}

void ServerThread::generateSignKey(const QByteArray &data){
    // 接收椭圆曲线参数a，b，p，gx，gy，用户私钥du，用户公钥Du = du^-1 * G
    emit consoleOutput("收到" + uid + "的签名密钥数据：" + data);
    QString tmpdata = data;
    QStringList datalist = tmpdata.split(",");
    int a = datalist.at(0).toInt();
    int b = datalist.at(1).toInt();
    int p = datalist.at(2).toInt();
    Point G(datalist.at(3).toInt(), datalist.at(4).toInt());
    EllipticCurve ec(a, b, p, G);
    printf("Elliptic curve parameters received.\n");
    int du = datalist.at(5).toInt();
    Point Du(datalist.at(6).toInt(), datalist.at(7).toInt());
    double time_start = (double) clock();
    int ds = rand() % (p - 2) + 1;// 服务端私钥ds
    Point Ds = ec.multiplyPoint(G, FermatInv(ds, p));// 服务端公钥Ds = ds^-1 * G
    Point tmp = ec.multiplyPoint(Du, FermatInv(ds, p));
    Point Ppub = ec.minusPoint(tmp, G);
    double time_end = (double) clock();
    emit consoleOutput("签名密钥已生成，耗时" + QString::number(time_end - time_start) + "ms");
    QStringList data2client;
    data2client << QString::number(Ds.x) << QString::number(Ds.y);
    emit sendData(data2client.join(",").toUtf8());
    emit consoleOutput("发回数据：" + data2client.join(",").toUtf8());
    // 更新数据库
    dbSetSignKey(db, uid, QString::number(du), Du, QString::number(ds), Ds, Ppub, ec.getParam());
    emit consoleOutput("签名密钥数据库已更新");
}

void ServerThread::generateSignature(const QByteArray &data){
    // 接收e，Ku
    QString tmpdata = data;
    QStringList datalist = tmpdata.split(",");
    int e = datalist.at(0).toInt();
    Point Ku(datalist.at(1).toInt(), datalist.at(2).toInt());
    QStringList ecparam = dbGetEllipticCurve(db, uid);// 调取椭圆曲线
    int p = ecparam.at(2).toInt();
    Point G(ecparam.at(3).toInt(), ecparam.at(4).toInt());
    EllipticCurve ec(ecparam.at(0).toInt(), ecparam.at(1).toInt(), p, G);
    double time_start = (double) clock();
    int ks = rand() % p;
    Point Ks = ec.multiplyPoint(G, ks);
    int ds = dbGetServerPrivateKey(db, uid).toInt();
    Point tmp = ec.plusPoint(Ks, Ku);
    int x = ec.multiplyPoint(tmp, FermatInv(ds, p)).x;
    int r = (x + e) % p;
    int s = (ds * r + ks) % p;
    double time_end = (double) clock();
    emit consoleOutput("服务端签名已生成，耗时" + QString::number(time_end - time_start) + "ms");
    QStringList data2client;
    data2client << QString::number(r) << QString::number(s);
    emit sendData(data2client.join(",").toUtf8());
    emit consoleOutput("发回数据：" + data2client.join(",").toUtf8());
    emit consoleOutput("服务端签名完成，签名值为(" + QString::number(r) + "," + QString::number(s) + ")");
//    printf("Server signature (r,s') = (%s,%s)\n", r.toString().c_str(), s.toString().c_str());
}

void ServerThread::splitEncryptKey(QString touid){
    emit consoleOutput("Generating encrypt key shard..");
    srand(time(0));
    // Shamir(3,4): f(x) = s + a1x + a2x^2 (mod p)
    int p = 907;
    BigNum s = BigNum::random(16) % (p - 2) + 1;
    BigNum a1 = BigNum::random(16) % (p - 2) + 1;
    BigNum a2 = BigNum::random(16) % (p - 2) + 1;
    BigNum x1 = BigNum::random(16) % (p - 2) + 1;
    BigNum y1 = (s + a1 * x1 + a2 * BinPow(x1, 2, p)) % p;
    BigNum x2 = BigNum::random(16) % (p - 2) + 1;
    BigNum y2 = (s + a1 * x2 + a2 * BinPow(x2, 2, p)) % p;
    BigNum x3 = BigNum::random(16) % (p - 2) + 1;
    BigNum y3 = (s + a1 * x3 + a2 * BinPow(x3, 2, p)) % p;
    BigNum x4 = BigNum::random(16) % (p - 2) + 1;
    BigNum y4 = (s + a1 * x4 + a2 * BinPow(x4, 2, p)) % p;
    // 发送密码数据
    QStringList data2client;
    data2client << QString::number(p) << x1.toString().c_str() << y1.toString().c_str();
    emit sendData(data2client.join(",").toUtf8());
    // 更新数据库存储
    dbSetEncryptKey(db, uid, touid, QString::number(p), QString::fromStdString(s.toString()),
                    QString::fromStdString(a1.toString()), QString::fromStdString(a2.toString()),
                    QString::fromStdString(x1.toString()), QString::fromStdString(y1.toString()),
                    QString::fromStdString(x2.toString()), QString::fromStdString(y2.toString()),
                    QString::fromStdString(x3.toString()), QString::fromStdString(y3.toString()),
                    QString::fromStdString(x4.toString()), QString::fromStdString(y4.toString()));
}

void ServerThread::sendSplitKey(const QByteArray &data){
    QString touid = data;
    QStringList datalist = dbGetClientEncryptKey(db, uid, touid);
    if(datalist.empty()){
        // 生成密钥并发送
        emit consoleOutput("No encrypt key in database.");
        splitEncryptKey(touid);
    }else{
        // 检查密码时效
        QDateTime begintime = QDateTime::fromString(datalist.at(3), "yyyy-MM-dd hh:mm:ss");
        QDateTime curtime = QDateTime::currentDateTime();
        int diff_day = begintime.secsTo(curtime)/86400;// 计算天数差
        if(diff_day >= 7){
            emit consoleOutput("Encrypt key invalid.");
            splitEncryptKey(touid);
            QStringList serverkey = dbGetServerEncryptKey(db, uid, touid);
            emit sendData(serverkey.join(",").toUtf8());
            emit consoleOutput("Encrypt key shard sent.");
        }else{
            // 发送已有的密钥
            emit consoleOutput("Encrypt key valid.");
            datalist << dbGetServerEncryptKey(db, uid, touid);
            emit sendData(datalist.join(",").toUtf8());
            emit consoleOutput("Encrypt key shard sent.");
        }
    }
}

void ServerThread::recvData(const QString ip, const QByteArray &data){
    emit consoleOutput("收到来自" + ip + "的消息：" + data);
    if(data.toStdString() == "login"){
        printf("Client %s logging in...\n", ip.toStdString().c_str());
        emit waitForData(VERIFYLOGIN);
    }else if(data.toStdString() == "gensignkey"){
        printf("Generating sign key with client...\n");
        emit waitForData(GENSIGNKEY);
    }else if(data.toStdString() == "signature"){
        printf("Generating signature with client...\n");
        emit waitForData(GENSIGNATURE);
    }else if(data.toStdString() == "getencryptkey"){
        printf("Sending sm4 key shard to client...\n");
        emit waitForData(SENDSPLITKEY);
    }else{
        QString msg = data;
        QString touid = msg.section(',', 0, 0);
        emit sendMessage(uid, touid, msg);
    }
}

void ServerThread::sendMessageSlot(QString uid, QString touid, const QString msg){
    // msg = "touid,message..."
    emit consoleOutput(uid + "发送给" + touid + "的消息：" + msg);
    QString data = "msg,";
    data.append(uid);
    data.append("," + msg);
    emit sendData(data.toUtf8());
}

void ServerThread::forwardMessageSlot(QString uid, QString touid, const QString msg){
    if(this->uid == touid) sendMessageSlot(uid, touid, msg);
}

void ServerThread::disconnectToHost(void){
    emit disconnectTCP(m_sockDesc);
    m_socket->disconnectFromHost();
    this->quit();
}
