#ifndef INCLUDED_H
#define INCLUDED_H

#include <QObject>
#include <QTcpServer>
#include <QUdpSocket>
#include <QPointer>
#include <QTcpSocket>
#include <QTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>
#include <QMap>
#include <QNetworkDatagram>
#include <QMetaEnum>


static const QString ip4Adress = "127.0.0.1"; //localhost
static const quint16 port = 1488;

namespace Json_key {
static const QString    TYPE = "type",
                        MESSAGE = "message",
                        FILE_NAME = "fileName",
                        FILE_SIZE = "fileSize",
                        FILE_EXIST = "fileIsExist",
                        MAX_PACKET_SIZE = "maxPacketSize",
                        CONFIRM = "confirm",
                        PACKET_NUMBER = "packetNumber";
}

namespace Json_type {
static const QString    SEND_MESSAGE = "sendMessage",
                        GET_FILE_INFO = "getFileInfo",
                        GET_FILE = "getFile",
                        CONFIRM_MAX_PACKET_SIZE = "confirmMaxPacketSize",
                        GET_NEXT_PACKET = "getNextPacket";

}



#endif // INCLUDED_H
