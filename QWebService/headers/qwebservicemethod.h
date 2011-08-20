#ifndef QWEBSERVICEMETHODABSTRACT_H
#define QWEBSERVICEMETHODABSTRACT_H

#include <QUrl>
#include <QObject>
#include "QWebService_global.h"
#include "qwebmethod.h"

// NOTE: this class is considered to be unecessary in this project, and may be deleted in the future.
class QWEBSERVICESHARED_EXPORT QWebServiceMethod : public QWebMethod
{
    Q_OBJECT

public:
    explicit QWebServiceMethod(QObject *parent = 0);
    QWebServiceMethod(QUrl hostUrl, QString messageName, QObject *parent = 0,
               Protocol protocol = soap12, HttpMethod httpMethod = POST);
    QWebServiceMethod(QString host, QString messageName, QObject *parent = 0,
               Protocol protocol = soap12, HttpMethod httpMethod = POST);
    QWebServiceMethod(QString host, QString messageName, QMap<QString, QVariant> params,
                 QObject *parent = 0, Protocol protocol = soap12, HttpMethod httpMethod = POST);

    using QWebMethod::sendMessage;
    bool sendMessage(QMap<QString, QVariant> params);
    QVariant static sendMessage(QObject *parent, QUrl url,
                                QString _messageName, QMap<QString, QVariant> params,
                                Protocol protocol = soap12, HttpMethod httpMethod = POST);

};

#endif // QWEBSERVICEMETHODABSTRACT_H
