#include "servertcp.h"

ServerTCP::ServerTCP(QObject *parent) : QObject(parent)
{
    tcpServer = new QTcpServer(this);
}

ServerTCP::~ServerTCP()
{
    tcpServer->deleteLater();
}

bool ServerTCP::createTcpServer()
{
    hostAdress.setAddress(ip4Adress);
    if(!tcpServer->listen(hostAdress, port)) {
        debugMessage("[Error] Can't create server " + tcpServer->errorString());
        tcpServer->close();
        return false;
    }
    connect(tcpServer, &QTcpServer::newConnection,
            this, &ServerTCP::newConnection);
    connect(tcpServer, QOverload<QAbstractSocket::SocketError>::of(&QTcpServer::acceptError),
            this, &ServerTCP::socketError);

    debugMessage("[Debug] Create TCP server is sucsessfull");
    return true;
}

void ServerTCP::closeServer()
{
    debugMessage("[Debug] Tcp server closed");
    tcpServer->close();
}

void ServerTCP::newConnection()
{
    debugMessage("[New connection]");

    QPointer<QTcpSocket> tcpSocket = tcpServer->nextPendingConnection();
    connect(tcpSocket, &QTcpSocket::disconnected,
            tcpSocket, &QTcpSocket::deleteLater);
    connect(tcpSocket, &QTcpSocket::readyRead,
            this, &ServerTCP::readMessage);
    connect(tcpSocket, &QTcpSocket::disconnected,
            this, &ServerTCP::disconnected);

    sendMessage(*tcpSocket, "You're connected to server!");
}

void ServerTCP::disconnected()
{
     debugMessage("[Debug] Socket is disconnected");
}


void ServerTCP::readMessage()
{
    QPointer<QTcpSocket> tcpSocket = (QTcpSocket*)sender();
    QJsonDocument jsonDockument = QJsonDocument::fromJson(tcpSocket->readAll());
    QJsonObject root = jsonDockument.object();
    QString type = root.value(Json_key::TYPE).toString();
    
    if(type == Json_type::SEND_MESSAGE) {
        debugMessage("[New message] " + root.value(Json_key::MESSAGE).toString());
    } else if(type == Json_type::GET_FILE_INFO) {
        sendFileInfo(*tcpSocket, root);
    } else if(type == Json_type::GET_FILE) {
        sendFile(*tcpSocket, root);
    } else {
        debugMessage("[Error] Undefinded command");
    }
}

void ServerTCP::sendMessage(QTcpSocket &tcpSocket, const QString &message)
{
    if(tcpSocket.isOpen()) {
        QJsonObject root;
        root.insert(Json_key::TYPE, Json_type::SEND_MESSAGE);
        root.insert(Json_key::MESSAGE, message);
        QJsonDocument jsonDocument(root);
        sendMessageUtf8(tcpSocket, jsonDocument.toJson(QJsonDocument::Compact));
    }
    else {
        debugMessage("[Error] Socket isn't open");
    }
}

void ServerTCP::sendMessageUtf8(QTcpSocket &tcpSocket, const QByteArray &message)
{
    if(tcpSocket.isOpen()) {
        tcpSocket.write(message);
    }
    else {
        debugMessage("[Error] Socket isn't open");
    }
}

bool ServerTCP::checkExistFile(const QString &fileName)
{
    return QFile::exists(fileName);
}

quint64 ServerTCP::checkFileSize(const QString &fileName)
{
    QFile file(fileName);
    return file.size();
}

void ServerTCP::sendFileInfo(QTcpSocket &tcpSocket, QJsonObject &root)
{
    QString fileName = root.value(Json_key::FILE_NAME).toString();
    debugMessage("[New request] Get file info: " + fileName);

    root.insert(Json_key::FILE_EXIST, checkExistFile(fileName));
    root.insert(Json_key::FILE_SIZE, QString::number(checkFileSize(fileName)));
    QJsonDocument jsonDocument(root);
    sendMessageUtf8(tcpSocket, jsonDocument.toJson(QJsonDocument::Compact));
}

void ServerTCP::sendFile(QTcpSocket &tcpSocket, QJsonObject &root)
{
    QString fileName = root.value(Json_key::FILE_NAME).toString();
    debugMessage("[New request] Get file: " + fileName);

    QFile file(fileName);
    file.open(QFile::ReadOnly);
    if(file.isOpen()) {
        sendMessageUtf8(tcpSocket, file.readAll());
    }
    else {
        debugMessage("[Error] Can't open file: " + fileName);
        tcpSocket.close();
    }
}

void ServerTCP::socketError(QAbstractSocket::SocketError error)
{
    QString message = "[Error] Can't connect socket: " + QString::number(error);
    debugMessage(message);
}


