#include "clientudp.h"

ClientUDP::ClientUDP(QObject *parent) : QObject(parent)
{
    udpSocket = new QUdpSocket(this);
    connect(udpSocket, &QUdpSocket::readyRead,
            this, &ClientUDP::readDatagram);
    connect(udpSocket, &QUdpSocket::disconnected,
            this, &ClientUDP::disconnected);
    connect(udpSocket, QOverload<QAbstractSocket::SocketError>::of(&QUdpSocket::errorOccurred),
                        this, &ClientUDP::socketError);
}

ClientUDP::~ClientUDP()
{
    udpSocket->deleteLater();
}

bool ClientUDP::createSocket()
{
    hostAdress.setAddress(ip4Adress);
    debugMessage("[Debug] Create UDP socket is sucsessfull");
    return true;
}

void ClientUDP::closeSocket()
{
    udpSocket->disconnected();
}

void ClientUDP::sendMessage(const QString &message)
{
    QJsonObject root;
    root.insert(Json_key::TYPE, Json_type::SEND_MESSAGE);
    root.insert(Json_key::MESSAGE, message);
    QJsonDocument jsonDocument(root);
    sendDatagram(jsonDocument.toJson(QJsonDocument::Compact));
}

void ClientUDP::getFile(const QString &fileName)
{
    this->fileName = fileName;
    QJsonObject root;
    root.insert(Json_key::TYPE, Json_type::GET_FILE_INFO);
    root.insert(Json_key::FILE_NAME, fileName);
    QJsonDocument jsonDocument(root);
    sendDatagram(jsonDocument.toJson(QJsonDocument::Compact));
}

void ClientUDP::sendDatagram(const QByteArray& bytes)
{
    udpSocket->writeDatagram(bytes, hostAdress, port);
}

void ClientUDP::loadedFile(const QNetworkDatagram& datagram)
{
    if(!file.isOpen()) {
        file.setFileName(fileName + "UDP.txt");
        if(!file.open(QFile::WriteOnly)) {
             debugMessage("[Error] Can't open file");
             isLoadedFile = false;
             return;
        }
        loadedSize = 0;
    }
    QByteArray data = datagram.data();
    file.write(data);
    loadedSize += data.size();
    debugMessage("[Debug] Loaded " + QString::number(loadedSize) + " from " + QString::number(fileSize));
    if(loadedSize == fileSize) {
        debugMessage("[Debug] File is download");
        isLoadedFile = false;
        file.close();
    }
    else {
        packetNumber++;
        getNextPacket();
    }
}

void ClientUDP::readDatagram()
{
    QPointer<QUdpSocket> udpSocket = (QUdpSocket*)sender();
    if(maxPacketSize < udpSocket->pendingDatagramSize()) {
        debugMessage("[Error] Datagram size is much more max size");
    }
    QNetworkDatagram datagram = udpSocket->receiveDatagram(maxPacketSize);
    if(!isLoadedFile) {
        checkMessage(datagram);
    }
    else {
        loadedFile(datagram);
    }
}

void ClientUDP::checkMessage(const QNetworkDatagram& datagram)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(datagram.data());
    QJsonObject root = jsonDocument.object();
    QString type = root.value(Json_key::TYPE).toString();

    if(type == Json_type::SEND_MESSAGE) {
        debugMessage("[New message] " + root.value(Json_key::MESSAGE).toString());
    } else if(type == Json_type::GET_FILE_INFO) {
        checkFileInfo(root);
    } else if(type == Json_type::CONFIRM_MAX_PACKET_SIZE) {
        checkConfirmMaxPacketSize(root);
    }
    else {
        debugMessage("[Error] Undefinded command");
    }
}

void ClientUDP::checkFileInfo(QJsonObject &root)
{
    if(root.value(Json_key::FILE_EXIST).toBool()) {
        fileSize = root.value(Json_key::FILE_SIZE).toVariant().toULongLong();
        debugMessage("[New answer] " + fileName + " size = " + QString::number(fileSize));

        root.find(Json_key::TYPE).value() = Json_type::CONFIRM_MAX_PACKET_SIZE;
        root.insert(Json_key::MAX_PACKET_SIZE, QString::number(maxPacketSize));
        QJsonDocument jsonDocument(root);
        sendDatagram(jsonDocument.toJson(QJsonDocument::Compact));
    }
    else
        debugMessage("[New answer] " + fileName + " isn't exist");
}

void ClientUDP::checkConfirmMaxPacketSize(QJsonObject &root)
{
    if(root.value(Json_key::CONFIRM).toBool()) {
        debugMessage("[New answer] Confirm max packet size = " + QString::number(maxPacketSize));
        isLoadedFile = true;
        packetNumber = 0;
        getNextPacket();
    } else {
        debugMessage("[Error] Max size packet not confirmed");
    }
}

void ClientUDP::getNextPacket()
{
    QJsonObject root;
    root.insert(Json_key::TYPE, Json_type::GET_NEXT_PACKET);
    root.insert(Json_key::FILE_NAME, fileName);
    root.insert(Json_key::PACKET_NUMBER, packetNumber);
    root.insert(Json_key::MAX_PACKET_SIZE, maxPacketSize);
    QJsonDocument jsonDocument(root);
    sendDatagram(jsonDocument.toJson(QJsonDocument::Compact));
}

void ClientUDP::socketError(QAbstractSocket::SocketError error)
{
    QString message = "[Error] Can't connect to server: " + udpSocket->errorString();
    debugMessage(message);
}

void ClientUDP::disconnected()
{
    debugMessage("[Debug] Socket is disconnected");
}
