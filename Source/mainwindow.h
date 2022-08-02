#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "servertcp.h"
#include "clienttcp.h"
#include "clientudp.h"
#include "serverudp.h"
#include "mybase64.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    QPointer<ServerTCP> serverTCP;
    QPointer<ClientTCP> clientTCP;
    QPointer<ClientUDP> clientUDP;
    QPointer<ServerUDP> serverUDP;
    Ui::MainWindow *ui;
private slots:
    void printDebugMessageServer(const QString& message);
    void printDebugMessageClient(const QString& message);
    void printDebugMessageClientUDP(const QString& message);
    void printDebugMessageServerUDP(const QString& message);
    void on_Connect_released();
    void on_MessageClient_returnPressed();
    void on_Disconnect_released();
    void on_CloseServer_released();
    void on_GetFile_released();
    void on_Connect_5_released();
    void on_Disconnect_5_released();
    void on_MessageClient_5_returnPressed();
    void on_GetFile_5_released();
    void on_decodeFile_released();
    void on_encodeFile_released();
    void on_closeUDP_released();
};
#endif // MAINWINDOW_H
