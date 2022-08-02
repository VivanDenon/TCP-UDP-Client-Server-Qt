#ifndef SERVERUDP_H
#define SERVERUDP_H

#include "Included.h"

class ServerUDP : public QObject
{
    Q_OBJECT
public:
    explicit ServerUDP(QObject *parent = nullptr);
    ~ServerUDP();
    bool createUDPSever();
    void closeServer();
private:
    QPointer<QUdpSocket> udpServerSocket;
    QHostAddress hostAdress;
    void sendMessage(const QString& message,  const QNetworkDatagram& recieveDatagram);
    void sendDatagram(const QByteArray& bytes, const QNetworkDatagram& recieveDatagram);
    bool checkExistFile(const QString& fileName);
    quint64 checkFileSize(const QString& fileName);
    void sendFileInfo(QJsonObject& root,  const QNetworkDatagram& recieveDatagram);
    void confirmMaxPacketSize(QJsonObject& root,  const QNetworkDatagram& recieveDatagram);
    void sendNextPacket(QJsonObject& root, const QNetworkDatagram& recieveDatagram);
    void readFile(QString fileName);
    qint64 maxPacketSize = 65000;
    QMap<quint16, qint64> maxPacketSizeForPorts;
    QFile file;
    QString fileData;
signals:
    void debugMessage(const QString& message);
private slots:
    void readMessage();
    void disconnected();
    void socketError(QAbstractSocket::SocketError error);
};

#endif // SERVERUDP_H
