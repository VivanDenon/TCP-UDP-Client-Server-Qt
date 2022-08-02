#ifndef SERVERTCP_H
#define SERVERTCP_H

#include "Included.h"

class ServerTCP : public QObject
{
    Q_OBJECT
public:
    explicit ServerTCP(QObject *parent = nullptr);
    ~ServerTCP();
    bool createTcpServer();
    void closeServer();
private:
    QPointer<QTcpServer> tcpServer;
    QHostAddress hostAdress;
    void sendMessage(QTcpSocket &tcpSocket, const QString& message);
    void sendMessageUtf8(QTcpSocket &tcpSocket, const QByteArray& message);
    bool checkExistFile(const QString& fileName);
    quint64 checkFileSize(const QString& fileName);
    void sendFileInfo(QTcpSocket &tcpSocket, QJsonObject &root);
    void sendFile(QTcpSocket &tcpSocket, QJsonObject &root);
signals:
    void debugMessage(const QString& message);
private slots:
    void newConnection();
    void disconnected();
    void readMessage();
    void socketError(QAbstractSocket::SocketError error);
};

#endif // SERVER_H
