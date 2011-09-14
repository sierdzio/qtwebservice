#ifndef QWEBMETHOD_H
#define QWEBMETHOD_H

#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qauthenticator.h>
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
        Http    = 0x01,
        Soap10  = 0x02,
        Soap12  = 0x04,
        Soap    = 0x06,
        Json    = 0x08,
        Xml     = 0x10,
        Rest    = 0x20
    };
    Q_DECLARE_FLAGS(Protocols, Protocol)

    enum HttpMethod
    {
        Post    = 0x1,
        Get     = 0x2,
        Put     = 0x4,
        Delete  = 0x8
    };

    explicit QWebMethod(QObject *parent = 0);
    QWebMethod(Protocol protocol = Soap12, HttpMethod httpMethod = Post,
               QObject *parent = 0);
    QWebMethod(QUrl url, Protocol protocol = Soap12, HttpMethod httpMethod = Post,
               QObject *parent = 0);
    ~QWebMethod();

    void setHost(QString newHost);
    void setHost(QUrl newHost);
    void setUsername(QString newUsername);
    void setPassword(QString newPassword);
    void setCredentials(QString newUsername, QString newPassword);
    void setMessageName(QString newName);
    void setMethodName(QString newName);
    void setParameters(QMap<QString, QVariant> params);
    void setReturnValue(QMap<QString, QVariant> returnValue);
    void setTargetNamespace(QString tNamespace);
    void setProtocol(Protocol protocol);
    void setHttpMethod(HttpMethod method);
    bool setHttpMethod(QString newMethod);
    // should this be changed to invokeMethod()?
    bool sendMessage(QByteArray requestData = QByteArray());
    bool authenticate(QString newUsername = QString(),
                      QString newPassword = QString());
    bool authenticate(QUrl customAuthString);

//    QVariant replyRead();
    QVariant replyReadParsed();
    QByteArray replyReadRaw();
    QString replyRead();
    QString methodName() const;
    QStringList parameterNames() const;
    QStringList returnValueName() const;
    QMap<QString, QVariant> parameterNamesTypes() const;
    QMap<QString, QVariant> returnValueNameType() const;
    QString targetNamespace() const;
    QString host() const;
    QUrl hostUrl() const;
    QString username() const;
    Protocol protocol() const;
    QString protocolString(bool includeRest = false) const;
    HttpMethod httpMethod() const;
    QString httpMethodString() const;
    QString errorInfo() const;
    bool isErrorState() const;
    bool isReplyReady() const;

signals:
    void replyReady(QByteArray rply);
    void errorEncountered(QString errMessage);

protected slots:
    void replyFinished(QNetworkReply *reply);
    void authReplyFinished(QNetworkReply *reply);
    void authenticationSlot(QNetworkReply *reply, QAuthenticator *authenticator);

protected:
    void init();
    void prepareRequestData();
    QString convertReplyToUtf(QString textToConvert);
    bool enterErrorState(QString errMessage = "");

    bool errorState;
    bool authReply;
    bool authenticationError;
    QString errorMessage;
    bool replyReceived;
    Protocol protocolUsed;
    HttpMethod httpMethodUsed;
    QUrl m_hostUrl;
    QString m_methodName;
    QString m_targetNamespace;
    QString m_username;
    QString m_password;
    QByteArray reply;
    QMap<QString, QVariant> parameters;
    QMap<QString, QVariant> returnValue;
    QNetworkAccessManager *manager;
    QByteArray data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QWebMethod::Protocols)

#endif // QWEBMETHOD_H
