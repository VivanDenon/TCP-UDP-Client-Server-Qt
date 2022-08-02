#ifndef CLIENTUDP_H
#define CLIENTUDP_H

#include "Included.h"

class ClientUDP : public QObject
{
    Q_OBJECT
public:
    explicit ClientUDP(QObject *parent = nullptr);
    ~ClientUDP();
    bool createSocket();
    void closeSocket();
    void sendMessage(const QString& message);
    void getFile(const QString& fileName);
private:
    QPointer<QUdpSocket> udpSocket;
    QHostAddress hostAdress;
    const qint64 maxPacketSize = 65000;
    qint32 packetNumber;
    quint64 fileSize, loadedSize;
    QString fileName;
    QFile file;
    bool isLoadedFile = false;
    void sendDatagram(const QByteArray& bytes);
    void loadedFile(const QNetworkDatagram& datagram);
    void checkMessage(const QNetworkDatagram& datagram);
    void checkFileInfo(QJsonObject &root);
    void checkConfirmMaxPacketSize(QJsonObject &root);
    void getNextPacket();
signals:
    void debugMessage(const QString& message);
private slots:
    void readDatagram();
    void socketError(QAbstractSocket::SocketError error);
    void disconnected();
};

#endif // CLIENTUDP_H
