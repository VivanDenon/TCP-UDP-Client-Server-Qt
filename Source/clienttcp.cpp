
#include "clienttcp.h"

ClientTCP::ClientTCP(QObject *parent) : QObject(parent)
{
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &ClientTCP::socketError);
    connect(tcpSocket, &QTcpSocket::connected,
            this, &ClientTCP::connected);
    connect(tcpSocket, &QTcpSocket::disconnected,
            this, &ClientTCP::disconnected);
    connect(tcpSocket, &QTcpSocket::readyRead,
            this, &ClientTCP::readMessage);
}

ClientTCP::~ClientTCP()
{
    tcpSocket->deleteLater();
}

void ClientTCP::createSocket()
{
    hostAdress.setAddress(ip4Adress);
    tcpSocket->connectToHost(hostAdress, port);
}

void ClientTCP::readMessage()
{
    if(!isLoadedFile) {
        checkMessage();
    }
    else {
        loadedFile();
    }

}

void ClientTCP::sendMessage(const QString &message)
{
    if(tcpSocket->isOpen()) {
        QJsonObject root;
        root.insert(Json_key::TYPE, Json_type::SEND_MESSAGE);
        root.insert(Json_key::MESSAGE, message);
        QJsonDocument jsonDocument(root);
        sendMessageUtf8(jsonDocument.toJson(QJsonDocument::Compact));
    }
    else {
        debugMessage("[Error] Socket isn't open");
    }

}

void ClientTCP::getFile(const QString &fileName)
{
    this->fileName = fileName;
    QJsonObject root;
    root.insert(Json_key::TYPE, Json_type::GET_FILE_INFO);
    root.insert(Json_key::FILE_NAME, fileName);
    QJsonDocument jsonDocument(root);
    sendMessageUtf8(jsonDocument.toJson(QJsonDocument::Compact));
}

void ClientTCP::sendMessageUtf8(const QByteArray &message)
{
    if(tcpSocket->isValid()) {
        tcpSocket->write(message);
    }
    else {
        debugMessage("[Error] Socket isn't open");
    }
}

void ClientTCP::loadedFile()
{
    if(!file.isOpen()) {
        file.setFileName(fileName + "TCP.txt");
        if(!file.open(QFile::WriteOnly)) {
             debugMessage("[Error] Can't open file");
             isLoadedFile = false;
             return;
        }
        loadedSize = 0;
    }
    QByteArray data = tcpSocket->readAll();
    file.write(data);
    loadedSize += data.size();
    debugMessage("[Debug] Loaded " + QString::number(loadedSize) + " from " + QString::number(fileSize));
    if(loadedSize == fileSize) {
        debugMessage("[Debug] File is download");
        isLoadedFile = false;
        file.close();
    }
}

void ClientTCP::checkMessage()
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(tcpSocket->readAll());
    QJsonObject root = jsonDocument.object();
    QString type = root.value(Json_key::TYPE).toString();

    if(type == Json_type::SEND_MESSAGE) {
        debugMessage("[New message] " + root.value(Json_key::MESSAGE).toString());
    } else if(type == Json_type::GET_FILE_INFO) {
        checkFileInfo(root);
    } else {
        debugMessage("[Error] Undefinded command");
    }
}

void ClientTCP::checkFileInfo(const QJsonObject &root)
{
    if(root.value(Json_key::FILE_EXIST).toBool()) {
        fileSize = root.value(Json_key::FILE_SIZE).toVariant().toULongLong();
        debugMessage("[New answer] " + fileName + " size = " + QString::number(fileSize));

        QJsonObject object;
        object.insert(Json_key::TYPE, Json_type::GET_FILE);
        object.insert(Json_key::FILE_NAME, fileName);
        QJsonDocument jsonDocument(object);
        sendMessageUtf8(jsonDocument.toJson(QJsonDocument::Compact));
        isLoadedFile = true;
    }
    else
        debugMessage("[New answer] " + fileName + " isn't exist");
}

void ClientTCP::disconnectFromServer()
{
    tcpSocket->disconnectFromHost();
}

void ClientTCP::socketError(QAbstractSocket::SocketError error)
{
    QString message = "[Error] Can't connect to server: " + tcpSocket->errorString();
    debugMessage(message);
}

void ClientTCP::connected()
{
    debugMessage("[Debug] Connect to server is sucsessfull");
}

void ClientTCP::disconnected()
{
    debugMessage("[Debug] Disconnected");
}


