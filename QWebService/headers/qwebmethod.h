#ifndef QWEBMETHOD_H
#define QWEBMETHOD_H

#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>
#include <QtCore/qmap.h>
#include <QtCore/qbytearray.h>
#include "QWebService_global.h"

class QWEBSERVICESHARED_EXPORT QWebMethod : public QObject
{
    Q_OBJECT
    Q_FLAGS(Protocols)
    Q_ENUMS(HttpMethod)

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

    QWebMethod(QObject *parent = 0, Protocol protocol = soap12, HttpMethod httpMethod = POST);
    ~QWebMethod();

    void setHost(QString newHost);
    void setHost(QUrl newHost);
    void setMessageName(QString newName);
    void setMethodName(QString newName);
    void setParameters(QMap<QString, QVariant> params);
    void setReturnValue(QMap<QString, QVariant> returnValue);
    void setTargetNamespace(QString tNamespace);
    void setProtocol(Protocol protocol);
    void setHttpMethod(HttpMethod method);
    bool sendMessage(QByteArray requestData = QByteArray());

    QVariant replyRead();
    QString messageName() const;
    QStringList parameterNames() const;
    QStringList returnValueName() const;
    QMap<QString, QVariant> parameterNamesTypes() const;
    QMap<QString, QVariant> returnValueNameType() const;
    QString targetNamespace() const;
    QString host() const;
    QUrl hostUrl() const;
    Protocol protocol() const;
    QString protocolString(bool includeRest = false) const;
    HttpMethod httpMethod() const;
    QString httpMethodString() const;
    QString errorInfo() const;
    bool isErrorState() const;
    bool isReplyReady() const;

signals:
    void replyReady(QVariant rply);
    void errorEncountered(QString errMessage);

protected slots:
    void replyFinished(QNetworkReply *reply);

protected: // Changed for 0.3.5, but precisely what should be protected and what private shall be decided later.
    void init();
    void prepareRequestData();
    QString convertReplyToUtf(QString textToConvert);
    bool enterErrorState(QString errMessage = "");

    bool errorState;
    QString errorMessage;
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
    QByteArray data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QWebMethod::Protocols)

#endif // QWEBMETHOD_H
