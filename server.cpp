#include "server.h"
#include "database.h"
#include "mainwindow.h"

Server::Server(QObject *parent) : QTcpServer(parent){
    m_mainWindow = dynamic_cast<MainWindow *>(parent);
    printf("Server created.\n");
    // 建立数据库连接
    dbCreateConnection(db, "keymanagement");
}

Server::~Server(){
}

QSqlDatabase Server::getDatabase(){
    return db;
}

void Server::incomingConnection(qintptr sockDesc){
    m_socketList.append(sockDesc);
    // **压力测试**
    ServerThread *thread = new ServerThread(sockDesc, db);
//    threadList.append(thread);

    m_mainWindow->showConnection(sockDesc);

    connect(thread, &ServerThread::disconnectTCP, m_mainWindow, &MainWindow::showDisconnection);
    connect(thread, &ServerThread::finished, thread, &ServerThread::deleteLater);
    connect(thread, &ServerThread::consoleOutput, this, &Server::showMessage);
    connect(thread, &ServerThread::sendMessage, this, &Server::sendMessageSlot);
    connect(thread, &ServerThread::addUidToMap, this, &Server::addUidToMapSlot);
    connect(this, &Server::forwardMessage, thread, &ServerThread::forwardMessageSlot);

    thread->start();
}

void Server::disconnectAll(){
    QList<ServerThread*> threadList = threadMap.keys();
    for(int i = 0; i < threadList.length(); i++){
        if(threadList[i]->isRunning()){
            threadList[i]->quit();
            threadList[i]->wait();
        }
    }
    close();
    showMessage("服务器停止监听，已断开所有客户端连接");
}

void Server::sendMessageSlot(QString uid, QString touid, const QString msg){
    emit forwardMessage(uid, touid, msg);
}

void Server::addUidToMapSlot(ServerThread* thread, QString uid){
    threadMap.insert(thread, uid);
    printf("threadMap insert %s\n", uid.toStdString().c_str());
}

void Server::showMessage(const QString msg){
    m_mainWindow->showMessage(msg);
}
