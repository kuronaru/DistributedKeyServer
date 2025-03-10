#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QEventLoop>
#include <QDebug>

#define VERIFYLOGIN 101
#define GENSIGNKEY 102
#define GENSIGNATURE 103
#define SENDSPLITKEY 104

class MySocket : public QTcpSocket {
    Q_OBJECT
public:
    explicit MySocket(qintptr sockDesc, QObject *parent = nullptr);
    ~MySocket();

signals:
    void dataReady(const QString &ip, const QByteArray &data);
    void sendData_verifyLogin(const QByteArray &data);
    void sendData_generateSignKey(const QByteArray &data);
    void sendData_generateSignature(const QByteArray &data);
    void sendData_sendSplitKey(const QByteArray &data);

public slots:
    void recvData(void);
    void recvSyncData(int type);
    void sendData(const QByteArray &data);

private:
    qintptr m_sockDesc;
};

#endif // MYSOCKET_H
