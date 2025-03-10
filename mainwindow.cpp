#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"
#include "database.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), m_server(new Server(this)), m_count(0){
    ui->setupUi(this);
    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startServer()));
    connect(ui->stopButton, SIGNAL(clicked()), m_server, SLOT(disconnectAll()));
    connect(ui->searchSignKeyButton, SIGNAL(clicked()), this, SLOT(showSignKey()));
    connect(ui->searchEncryptKeyButton, SIGNAL(clicked()), this, SLOT(showEncryptKey()));
}

MainWindow::~MainWindow(){
    delete ui;
    delete m_server;
}

void MainWindow::startServer(){
    showMessage("服务器已启动");
    QHostAddress ip;
    if(ui->ipEdit->text().isEmpty()) ip = QHostAddress::Any;
    else ip = QHostAddress(ui->ipEdit->text());
    int port = ui->portEdit->text().toInt();
    m_server->listen(ip, port);
    showMessage("正在监听地址" + ip.toString() + "，端口" + ui->portEdit->text());
}

void MainWindow::showMessage(QString msg){
    ui->textBrowser->append(QTime::currentTime().toString("hh:mm:ss") + ": " + msg);
}

void MainWindow::showConnection(int sockDesc){
    m_count++;
    showMessage("客户端" + QString("%1").arg(sockDesc) + "已连接，现有" + QString("%1").arg(m_count) + "个连接的客户端");
    printf("Client %d connected, %d clients connected.\n", sockDesc, m_count);
}

void MainWindow::showDisconnection(int sockDesc){
    m_count--;
    showMessage("客户端" + QString("%1").arg(sockDesc) + "已断开，现有" + QString("%1").arg(m_count) + "个连接的客户端");
    printf("Client %d disconnected, %d clients connected.\n", sockDesc, m_count);
}

void MainWindow::showSignKey(){
    QString uid = ui->usernameEdit->text();
    printf("Searching sign key, UID: %s.\n", uid.toStdString().c_str());
    QStringList data = dbSearchSignKey(m_server->getDatabase(), uid);
    if(!data.isEmpty()){
        for(int i = 0; i < 14; i++){
            ui->signKeyTableWidget->setItem(i - 1, 1, new QTableWidgetItem(data.at(i)));
        }
    }else{
        printf("Sign key of UID: %s is not found.\n", uid.toStdString().c_str());
    }
}

void MainWindow::showEncryptKey(){
    QString uid = ui->uidEdit->text();
    QString touid = ui->touidEdit->text();
    printf("Searching encrypt key, UID: %s to %s.\n", uid.toStdString().c_str(), touid.toStdString().c_str());
    QStringList data = dbSearchEncryptKey(m_server->getDatabase(), uid, touid);
    if(!data.isEmpty()){
        for(int i = 0; i < 13; i++){
            ui->encryptKeyTableWidget->setItem(i - 1, 1, new QTableWidgetItem(data.at(i)));
        }
    }else{
        printf("Encrypt key of UID: %s to %s is not found.\n", uid.toStdString().c_str(), touid.toStdString().c_str());
    }
}
