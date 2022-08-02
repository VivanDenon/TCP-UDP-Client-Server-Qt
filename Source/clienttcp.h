#ifndef CLIENTTCP_H
#define CLIENTTCP_H

#include "Included.h"

class ClientTCP : public QObject
{
    Q_OBJECT
public:
    explicit ClientTCP(QObject *parent = nullptr);
    ~ClientTCP();
    void createSocket();
    void disconnectFromServer();
    void sendMessage(const QString& message);
    void getFile(const QString& fileName);
private:
    QPointer<QTcpSocket> tcpSocket;
    QHostAddress hostAdress;
    quint64 fileSize, loadedSize;
    QString fileName;
    QFile file;
    bool isLoadedFile = false;
    void sendMessageUtf8(const QByteArray& message);
    void loadedFile();
    void checkMessage();
    void checkFileInfo(const QJsonObject &root);
signals:
    void debugMessage(const QString& message);
private slots:
    void readMessage();
    void socketError(QAbstractSocket::SocketError error);
    void connected();
    void disconnected();
};

#endif // CLIENTTCP_H
