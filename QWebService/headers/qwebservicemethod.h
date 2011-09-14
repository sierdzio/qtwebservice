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
    explicit QWebServiceMethod(QObject *parent = 0, Protocol protocol = Soap12,
                               HttpMethod httpMethod = Post);
    QWebServiceMethod(QUrl hostUrl, QString methodName, QObject *parent = 0,
                      Protocol protocol = Soap12, HttpMethod httpMethod = Post);
    QWebServiceMethod(QString host, QString methodName, QObject *parent = 0,
                      Protocol protocol = Soap12, HttpMethod httpMethod = Post);
    QWebServiceMethod(QString host, QString methodName,  QString targetNamespace,
                      QMap<QString, QVariant> params, QObject *parent = 0,
                      Protocol protocol = Soap12, HttpMethod httpMethod = Post);

    using QWebMethod::sendMessage;
    bool sendMessage(QMap<QString, QVariant> params);
    QByteArray static sendMessage(QObject *parent, QUrl url,
                                QString methodName, QString targetNamespace,
                                QMap<QString, QVariant> params,
                                Protocol protocol = Soap12, HttpMethod httpMethod = Post);

};

#endif // QWEBSERVICEMETHOD_H
