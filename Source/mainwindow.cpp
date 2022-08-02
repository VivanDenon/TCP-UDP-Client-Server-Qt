#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serverTCP = new ServerTCP();
    clientTCP = new ClientTCP();
    clientUDP = new ClientUDP();
    serverUDP = new ServerUDP();

    connect(serverTCP, &ServerTCP::debugMessage,
            this, &MainWindow::printDebugMessageServer);
    connect(clientTCP, &ClientTCP::debugMessage,
            this, &MainWindow::printDebugMessageClient);
    connect(clientUDP, &ClientUDP::debugMessage,
            this, &MainWindow::printDebugMessageClientUDP);
    connect(serverUDP, &ServerUDP::debugMessage,
            this, &MainWindow::printDebugMessageServerUDP);

    serverTCP->createTcpServer();
    serverUDP->createUDPSever();
}

MainWindow::~MainWindow()
{
    serverTCP->deleteLater();
    clientTCP->deleteLater();
    clientUDP->deleteLater();
    serverUDP->deleteLater();
    delete ui;
}

void MainWindow::printDebugMessageServer(const QString &message)
{
    ui->ServerDebug->append("{" + QTime::currentTime().toString() + "} : " + message);
}

void MainWindow::printDebugMessageClient(const QString &message)
{
    ui->ClientDebug->append("{" + QTime::currentTime().toString() + "} : " + message);
}

void MainWindow::printDebugMessageClientUDP(const QString &message)
{
    ui->ClientDebug_5->append("{" + QTime::currentTime().toString() + "} : " + message);
}

void MainWindow::printDebugMessageServerUDP(const QString &message)
{
    ui->ServerUDPDebug->append("{" + QTime::currentTime().toString() + "} : " + message);
}




void MainWindow::on_Connect_released()
{
    clientTCP->createSocket();
}

void MainWindow::on_MessageClient_returnPressed()
{
    clientTCP->sendMessage(ui->MessageClient->text());
    ui->MessageClient->clear();
}

void MainWindow::on_Disconnect_released()
{
    clientTCP->disconnectFromServer();
}

void MainWindow::on_CloseServer_released()
{
    serverTCP->closeServer();
}

void MainWindow::on_GetFile_released()
{
    clientTCP->getFile(ui->MessageClient->text());
    ui->MessageClient->clear();
}

void MainWindow::on_Connect_5_released()
{
    clientUDP->createSocket();
}

void MainWindow::on_Disconnect_5_released()
{
    clientUDP->closeSocket();
}

void MainWindow::on_MessageClient_5_returnPressed()
{
    clientUDP->sendMessage(ui->MessageClient_5->text());
    ui->MessageClient_5->clear();
}

void MainWindow::on_GetFile_5_released()
{
    clientUDP->getFile(ui->MessageClient_5->text());
    ui->MessageClient_5->clear();
}

void MainWindow::on_decodeFile_released()
{
    QFile fileToDecode("ToDecode.txt"), fileDecoded("Decoded.txt");
    if(fileToDecode.open(QFile::ReadOnly) && fileDecoded.open(QFile::WriteOnly)) {
        QString line;
        line = fileToDecode.readLine();
        while(line != nullptr) {
            QString decodeLine = MyBase64::fromBase64(line.trimmed().toUtf8()) + '\n';
            fileDecoded.write(decodeLine.toLatin1());
            line = fileToDecode.readLine();
        }
    }
    else {
        qDebug() << "Error::File isn't open";
    }
}

void MainWindow::on_encodeFile_released()
{
    QFile fileToEncode("ToEncode.txt"), fileEncoded("Encoded.txt");
    if(fileToEncode.open(QFile::ReadOnly) && fileEncoded.open(QFile::WriteOnly)) {
        QString line;
        line = fileToEncode.readLine();
        while(line != nullptr) {
            fileEncoded.write(MyBase64::toBase64(line.trimmed().toUtf8()) + '\n');
            line = fileToEncode.readLine();
        }
    }
    else {
        qDebug() << "Error::File isn't open";
    }
}

void MainWindow::on_closeUDP_released()
{
    serverUDP->closeServer();
}
