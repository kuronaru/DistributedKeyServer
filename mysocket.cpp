#include "mysocket.h"

MySocket::MySocket(qintptr sockDesc, QObject *parent) : QTcpSocket(parent), m_sockDesc(sockDesc) {
    connect(this, &MySocket::readyRead, this, &MySocket::recvData);
}

MySocket::~MySocket(){
}

void MySocket::recvData(void){
    QString ip = peerAddress().toString().remove(0, 7);
    QByteArray data = readAll();
    emit dataReady(ip, data);
}

void MySocket::recvSyncData(int type){
    QString data;
    printf("Socket waiting for readyRead, type %d...\n", type);
    // 同步通信
    QEventLoop loop;
    disconnect(this, &MySocket::readyRead, this, &MySocket::recvData);
    connect(this, &MySocket::readyRead, &loop, &QEventLoop::quit);
    loop.exec();
    data = readAll();
    printf("Socket read data: %s\n", data.toStdString().c_str());
    connect(this, &MySocket::readyRead, this, &MySocket::recvData);
    disconnect(this, &MySocket::readyRead, &loop, &QEventLoop::quit);
    switch(type){
        case VERIFYLOGIN :emit sendData_verifyLogin(data.toUtf8());break;
        case GENSIGNKEY :emit sendData_generateSignKey(data.toUtf8());break;
        case GENSIGNATURE :emit sendData_generateSignature(data.toUtf8());break;
        case SENDSPLITKEY :emit sendData_sendSplitKey(data.toUtf8());break;
    }
}

void MySocket::sendData(const QByteArray &data){
    if (!data.isEmpty()){
        write(data);
    }
}
