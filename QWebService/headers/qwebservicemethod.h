#ifndef QWEBSERVICEMETHOD_H
#define QWEBSERVICEMETHOD_H

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include "QWebService_global.h"
#include "qwebmethod.h"

class QWEBSERVICESHARED_EXPORT QWebServiceMethod : public QWebMethod
{
    Q_OBJECT

public:
    explicit QWebServiceMethod(QObject *parent = 0, Protocol protocol = soap12, HttpMethod httpMethod = POST);
    QWebServiceMethod(QUrl hostUrl, QString metodName, QObject *parent = 0,
                      Protocol protocol = soap12, HttpMethod httpMethod = POST);
    QWebServiceMethod(QString host, QString metodName, QObject *parent = 0,
                      Protocol protocol = soap12, HttpMethod httpMethod = POST);
    QWebServiceMethod(QString host, QString metodName,  QString targetNamespace,
                      QMap<QString, QVariant> params, QObject *parent = 0,
                      Protocol protocol = soap12, HttpMethod httpMethod = POST);

    using QWebMethod::sendMessage;
    bool sendMessage(QMap<QString, QVariant> params);
    QByteArray static sendMessage(QObject *parent, QUrl url,
                                QString metodName, QString targetNamespace,
                                QMap<QString, QVariant> params,
                                Protocol protocol = soap12, HttpMethod httpMethod = POST);

};

#endif // QWEBSERVICEMETHOD_H
