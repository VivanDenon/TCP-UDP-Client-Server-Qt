#include "serverudp.h"

ServerUDP::ServerUDP(QObject *parent) : QObject(parent)
{
    udpServerSocket = new QUdpSocket(this);
}

ServerUDP::~ServerUDP()
{
    udpServerSocket->deleteLater();
}

bool ServerUDP::createUDPSever()
{
    hostAdress.setAddress(ip4Adress);
    if(!udpServerSocket->bind(hostAdress, port)) {
        debugMessage("[Error] Can't create server " + udpServerSocket->errorString());
        udpServerSocket->close();
        return false;
    }
    connect(udpServerSocket, &QUdpSocket::readyRead,
            this, &ServerUDP::readMessage);
    connect(udpServerSocket, &QUdpSocket::disconnected,
            this, &ServerUDP::disconnected);
    debugMessage("[Debug] Create UDP server is sucsessfull");
    return true;
}

void ServerUDP::closeServer()
{
    debugMessage("[Debug] UDP server closed");
    udpServerSocket->close();
}

void ServerUDP::sendMessage(const QString &message, const QNetworkDatagram& recieveDatagram)
{
    QJsonObject root;
    root.insert(Json_key::TYPE, Json_type::SEND_MESSAGE);
    root.insert(Json_key::MESSAGE, message);
    QJsonDocument jsonDocument(root);  
    sendDatagram(jsonDocument.toJson(QJsonDocument::Compact), recieveDatagram);
}

void ServerUDP::sendDatagram(const QByteArray& bytes,
                             const QNetworkDatagram& recieveDatagram)
{
    udpServerSocket->writeDatagram(bytes, recieveDatagram.senderAddress(),
                            recieveDatagram.senderPort());
}

bool ServerUDP::checkExistFile(const QString &fileName)
{
    return QFile::exists(fileName);
}

quint64 ServerUDP::checkFileSize(const QString &fileName)
{
    QFile file(fileName);
    return file.size();
}

void ServerUDP::sendFileInfo(QJsonObject &root, const QNetworkDatagram& recieveDatagram)
{
    QString fileName = root.value(Json_key::FILE_NAME).toString();
    debugMessage("[New request] Get file info: " + fileName);

    root.insert(Json_key::FILE_EXIST, checkExistFile(fileName));
    root.insert(Json_key::FILE_SIZE, QString::number(checkFileSize(fileName)));
    QJsonDocument jsonDocument(root);
    sendDatagram(jsonDocument.toJson(QJsonDocument::Compact), recieveDatagram);
}

void ServerUDP::confirmMaxPacketSize(QJsonObject& root, const QNetworkDatagram& recieveDatagram)
{
    qint64 maxPacketSize = root.value(Json_key::MAX_PACKET_SIZE).toVariant().toLongLong();
    debugMessage("[New request] Confirm max size: " + QString::number(maxPacketSize));

    bool isConfirm = maxPacketSize <= this->maxPacketSize;
    if(isConfirm) {
        maxPacketSizeForPorts.insert(recieveDatagram.senderPort(), maxPacketSize);
        readFile(root.value(Json_key::FILE_NAME).toString());
    } else {
        debugMessage("[Error] Max packet size is much more max size");
    }
    root.find(Json_key::TYPE).value() = Json_type::CONFIRM_MAX_PACKET_SIZE;
    root.insert(Json_key::CONFIRM, isConfirm);
    QJsonDocument jsonDocument(root);
    sendDatagram(jsonDocument.toJson(QJsonDocument::Compact), recieveDatagram);
}

void ServerUDP::sendNextPacket(QJsonObject &root, const QNetworkDatagram &recieveDatagram)
{
    QString fileName = root.value(Json_key::FILE_NAME).toString();
    debugMessage("[New request] Get packet: " + QString::number(root.value(Json_key::PACKET_NUMBER).toInt())
                 + " of " + fileName);

    qint64 packetSize = root.value(Json_key::MAX_PACKET_SIZE).toVariant().toLongLong();
    qint32 packetNumber = root.value(Json_key::PACKET_NUMBER).toVariant().toInt();
    sendDatagram(fileData.mid(packetSize * packetNumber, packetSize).toLatin1(), recieveDatagram);
}

void ServerUDP::readFile(QString fileName)
{
    file.setFileName(fileName);
    if(file.open(QFile::ReadOnly)) {
        fileData.clear();
        fileData = file.readAll();
    }
    else {
        debugMessage("[Error] Can't open file: " + fileName);
    }
}

void ServerUDP::readMessage()
{
    if(maxPacketSize < udpServerSocket->pendingDatagramSize()) {
        debugMessage("[Error] Datagram size is much more max size");
    }
    QNetworkDatagram datagram = udpServerSocket->receiveDatagram(maxPacketSize);
    QJsonDocument jsonDockument = QJsonDocument::fromJson(datagram.data());
    QJsonObject root = jsonDockument.object();
    QString type = root.value(Json_key::TYPE).toString();

    if(type == Json_type::SEND_MESSAGE) {
        debugMessage("[New message] " + root.value(Json_key::MESSAGE).toString());
    } else if(type == Json_type::GET_FILE_INFO) {
        sendFileInfo(root, datagram);
    } else if(type == Json_type::CONFIRM_MAX_PACKET_SIZE) {
        confirmMaxPacketSize(root, datagram);
    } else if(type == Json_type::GET_NEXT_PACKET) {
        sendNextPacket(root, datagram);
    } else {
        debugMessage("[Error] Undefinded command");
    }
}

void ServerUDP::disconnected()
{
    debugMessage("[Debug] Socket is disconnected");
}

void ServerUDP::socketError(QAbstractSocket::SocketError error)
{
    QString message = "[Error] Can't connect socket: " + QString::number(error);
    debugMessage(message);
}
