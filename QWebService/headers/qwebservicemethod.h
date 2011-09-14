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
    explicit QWebServiceMethod(QObject *parent = 0);
    QWebServiceMethod(Protocol protocol = Soap12, HttpMethod httpMethod = Post,
                               QObject *parent = 0);
    QWebServiceMethod(QUrl hostUrl, QString methodName, Protocol protocol = Soap12,
                      HttpMethod httpMethod = Post, QObject *parent = 0);
    QWebServiceMethod(QString host, QString methodName, Protocol protocol = Soap12,
                      HttpMethod httpMethod = Post, QObject *parent = 0);
    QWebServiceMethod(QString host, QString methodName,  QString targetNamespace,
                      QMap<QString, QVariant> params, Protocol protocol = Soap12,
                      HttpMethod httpMethod = Post, QObject *parent = 0);

    using QWebMethod::sendMessage;
    bool sendMessage(QMap<QString, QVariant> params);
    QByteArray static sendMessage(QUrl url,
                                  QString methodName, QString targetNamespace,
                                  QMap<QString, QVariant> params,
                                  Protocol protocol = Soap12, HttpMethod httpMethod = Post,
                                  QObject *parent = 0);

};

#endif // QWEBSERVICEMETHOD_H
