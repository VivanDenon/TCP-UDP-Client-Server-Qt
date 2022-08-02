#include "mybase64.h"

const QByteArray MyBase64::base64ABC = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                        "abcdefghijklmnopqrstuvwxyz"
                                        "0123456789+/";

MyBase64::MyBase64()
{

}

QByteArray MyBase64::toBase64(const QByteArray &bytesToEncode)
{
    QByteArray result;
    int size = bytesToEncode.size(),
            remainder = size % 3;
    size -= remainder;
    for(int i = 0; i < size; i+= 3) {
        int a = bytesToEncode[i] >> 2,
            b = ((bytesToEncode[i] << 4) | (bytesToEncode[i + 1] >> 4)) & 63,
            c = ((bytesToEncode[i + 1] << 2) | (bytesToEncode[i + 2] >> 6)) & 63,
            d = bytesToEncode[i + 2] & 63;
        result += base64ABC[a];
        result += base64ABC[b];
        result += base64ABC[c];
        result += base64ABC[d];
    }
    if(remainder > 0) {
        int a = bytesToEncode[size] >> 2;
        result += base64ABC[a];
        if(remainder > 1) {
            int b = ((bytesToEncode[size] << 4) | (bytesToEncode[size + 1] >> 4)) & 63,
                c = ((bytesToEncode[size + 1] << 2) | ('\0' >> 6)) & 63;
            result += base64ABC[b];
            result += base64ABC[c];
        }
        else {
            int b = ((bytesToEncode[size] << 4) | ('\0' >> 4)) & 63;
            result += base64ABC[b];
            result += '=';
        }
        result += '=';
    }
    return result;
}

QByteArray MyBase64::fromBase64(const QByteArray &bytesToDecode)
{
    QByteArray result;
    int size = bytesToDecode.size();
    if(size % 4 != 0) {
        return nullptr;
    }

    int i = 0, j = 0;
    char indexes[4];
    while(j < size && bytesToDecode[j] != '=') {
        indexes[i] = base64ABC.indexOf(bytesToDecode[j]);
        i++; j++;
        if(i == 4) {
            result += (indexes[0] << 2) | (indexes[1] >> 4);
            result += (indexes[1] << 4) | (indexes[2] >> 2);
            result += (indexes[2] << 6) | (indexes[3]);
            i = 0;
        }
    }
    if(i > 0) {
        result += (indexes[0] << 2) | (indexes[1] >> 4);
        if(i > 2)
            result += (indexes[1] << 4) | (indexes[2] >> 2);
    }
    qDebug() << result;
    return result;
}
