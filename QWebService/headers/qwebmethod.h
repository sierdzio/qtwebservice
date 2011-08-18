#ifndef QWebMethod_H
#define QWebMethod_H

#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "QWebService_global.h"

class QWEBSERVICESHARED_EXPORT QWebMethod : public QObject
{
    Q_OBJECT
    Q_ENUMS(Protocol)
    Q_FLAGS(Protocols)
    Q_ENUMS(HttpMethod)
    Q_FLAGS(HttpMethods)

public:
    enum Protocol
    {
        http    = 0x01,
        soap10  = 0x02,
        soap12  = 0x04,
        soap    = 0x06,
        json    = 0x08,
        xml     = 0x10,
        rest    = 0x20
    };
    Q_DECLARE_FLAGS(Protocols, Protocol)

    enum HttpMethod
    {
        POST    = 0x1,
        GET     = 0x2,
        PUT     = 0x4,
        DELETE  = 0x8
    };
    Q_DECLARE_FLAGS(HttpMethods, HttpMethod)

    explicit QWebMethod(QObject *parent = 0, Protocol protocol = soap12, HttpMethod httpMethod = POST);
    QWebMethod(QUrl hostUrl, QString messageName, QObject *parent = 0,
               Protocol protocol = soap12, HttpMethod httpMethod = POST);
    QWebMethod(QString host, QString messageName, QObject *parent = 0,
               Protocol protocol = soap12, HttpMethod httpMethod = POST);
    QWebMethod(QString host, QString messageName, QMap<QString, QVariant> params,
                 QObject *parent = 0, Protocol protocol = soap12, HttpMethod httpMethod = POST);
    ~QWebMethod();

    void setHost(QString newHost);
    void setHost(QUrl newHost);
    void setMessageName(QString newName);
    void setParameters(QMap<QString, QVariant> params);
    void setReturnValue(QMap<QString, QVariant> returnValue);
    void setTargetNamespace(QString tNamespace);
    void setProtocol(Protocol protocol);
    void setHttpMethod(HttpMethod method);
    bool sendMessage();
    bool sendMessage(QMap<QString, QVariant> params);
    QVariant static sendMessage(QObject *parent, QUrl url,
                                QString _messageName, QMap<QString, QVariant> params,
                                Protocol protocol = soap12, HttpMethod httpMethod = POST);
//    QVariant static sendRestMessage(QObject *parent, QUrl fullPath, QString parameters,
//                                Protocol protocol = soap12, HttpMethod httpMethod = POST);
    QVariant replyRead();
    QString messageName();
    QStringList parameterNames() const;
    QStringList returnValueName() const;
    QMap<QString, QVariant> parameterNamesTypes() const;
    QMap<QString, QVariant> returnValueNameType() const;
    QString targetNamespace();
    QString host();
    QUrl hostUrl();
    Protocol protocol();
    HttpMethod httpMethod();

signals:
    void replyReady(QVariant rply);

public slots:
    void replyFinished(QNetworkReply *reply);

protected: // Changed for 0.3.5, but precisely what should be protected and what private shall be decided later.
    void init();
    virtual void configure();
    void prepareRequestData();
    QString convertReplyToUtf(QString textToConvert);

private:
    bool replyReceived;
    Protocol protocolUsed;
    HttpMethod httpMethodUsed;
    QUrl m_hostUrl;
    QString m_messageName;
    QString m_targetNamespace;
    QVariant reply;
    QMap<QString, QVariant> parameters;
    QMap<QString, QVariant> returnValue;
    QNetworkAccessManager *manager;
    QNetworkReply *networkReply;
    QByteArray data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QWebMethod::Protocols)
Q_DECLARE_OPERATORS_FOR_FLAGS(QWebMethod::HttpMethods)

#endif // QWebMethod_H
