#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Server;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startServer(void);
    void showMessage(QString msg);
    void showConnection(int sockDesc);
    void showDisconnection(int sockDesc);
    void showSignKey(void);
    void showEncryptKey(void);

private:
    Ui::MainWindow *ui;
    friend class Server;
    Server *m_server;
    int m_count;
};

#endif // MAINWINDOW_H
