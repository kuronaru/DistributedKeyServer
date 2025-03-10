#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include "serverthread.h"

class MainWindow;

class Server : public QTcpServer {
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();
    QSqlDatabase getDatabase(void);

private:
    void incomingConnection(qintptr sockDesc);

signals:
    void forwardMessage(QString uid, QString touid, const QString msg);

private slots:
    void disconnectAll(void);
    void sendMessageSlot(QString uid, QString touid, const QString msg);
    void addUidToMapSlot(ServerThread* thread, const QString uid);
    void showMessage(const QString msg);

private:
    MainWindow *m_mainWindow;
    QSqlDatabase db;
    QList<int> m_socketList;
    QMap<ServerThread*, QString> threadMap;
};

#endif // SERVER_H
