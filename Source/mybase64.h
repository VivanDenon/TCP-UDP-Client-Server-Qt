#ifndef MYBASE64_H
#define MYBASE64_H
#include "Included.h"

class MyBase64
{
public:
    MyBase64();
    static QByteArray toBase64(const QByteArray& bytesToEncode);
    static QByteArray fromBase64(const QByteArray& bytesToDecode);
private:
    static const QByteArray base64ABC;
};

#endif // MYBASE64_H
