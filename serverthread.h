#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include <QDateTime>
#include <QSqlDatabase>
#include <sys/time.h>
#include "mysocket.h"

class ServerThread : public QThread {
    Q_OBJECT
public:
    ServerThread(qintptr sockDesc, QSqlDatabase db, QObject *parent = nullptr);
    ~ServerThread();
    QString getUid(void);

private:
    void run(void);

signals:
    void sendData(const QByteArray &data);
    void waitForData(int type);
    void sendMessage(QString uid, QString touid, const QString msg);
    void addUidToMap(ServerThread* thread, const QString uid);
    void disconnectTCP(qintptr sockDesc);
    void consoleOutput(const QString msg);

public slots:
    void forwardMessageSlot(QString uid, QString touid, const QString msg);// 转发来自服务器的消息

private slots:
    void recvData(const QString ip, const QByteArray &data);// 接收Socket的消息
    void sendMessageSlot(QString uid, QString touid, const QString msg);// 向用户发送消息
    void verifyLogin(const QByteArray &data);// 登录认证
    void generateSignKey(const QByteArray &data);// 协同签名密钥生成
    void generateSignature(const QByteArray &data);// 协同签名生成
    void splitEncryptKey(QString touid);// 密钥分割加密密钥生成
    void sendSplitKey(const QByteArray &data);// 发送密钥碎片
    void disconnectToHost(void);

private:
    MySocket *m_socket;
    QSqlDatabase db;
    QString uid;
    qintptr m_sockDesc;
};

#endif // SERVERTHREAD_H
