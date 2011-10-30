/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the QWebService library.
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.txt included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

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
#include <QtCore/qdatetime.h>
#include <QtCore/qcoreapplication.h>
#include "QWebService_global.h"

class QWebMethodPrivate;

class QWEBSERVICESHARED_EXPORT QWebMethod : public QObject
{
    Q_OBJECT
    Q_FLAGS(Protocols)
    Q_ENUMS(HttpMethod)

    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(QUrl hostUrl READ hostUrl WRITE setHost NOTIFY hostUrlChanged)
    Q_PROPERTY(QString name READ methodName WRITE setMethodName NOTIFY nameChanged)
    Q_PROPERTY(QString targetNamespace READ targetNamespace WRITE setTargetNamespace NOTIFY targetNamespaceChanged)
    Q_PROPERTY(QStringList parameterNames READ parameterNames NOTIFY parameterNamesChanged)
    Q_PROPERTY(QString protocol READ protocol WRITE setProtocol NOTIFY protocolChanged)
    Q_PROPERTY(QString httpMethod READ httpMethod WRITE setHttpMethod NOTIFY httpMethodChanged)

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

    explicit QWebMethod(QObject *parent = 0,
                        Protocol protocol = Soap12,
                        HttpMethod httpMethod = Post);
    QWebMethod(const QUrl &url,
               Protocol protocol = Soap12,
               HttpMethod httpMethod = Post,
               QObject *parent = 0);
    ~QWebMethod();

    QString host() const;
    QUrl hostUrl() const;
    void setHost(const QString &newHost);
    void setHost(const QUrl &newHost);

    QString username() const;
    void setUsername(const QString &newUsername);
    void setPassword(const QString &newPassword);
    void setCredentials(const QString &newUsername, const QString &newPassword);
    bool authenticate(const QString &newUsername = QString(),
                      const QString &newPassword = QString());
    bool authenticate(const QUrl &customAuthString);

    QString methodName() const;
    void setMethodName(const QString &newName);

    QStringList parameterNames() const;
    QMap<QString, QVariant> parameterNamesTypes() const;
    void setParameters(const QMap<QString, QVariant> &params);

    QStringList returnValueName() const;
    QMap<QString, QVariant> returnValueNameType() const;
    void setReturnValue(const QMap<QString, QVariant> &returnValue);

    QString targetNamespace() const;
    void setTargetNamespace(const QString &tNamespace);

    Protocol protocol() const;
    QString protocolString(bool includeRest = false) const;
    bool setProtocol(Protocol protocol);
    bool setProtocol(QString protocolString);

    HttpMethod httpMethod() const;
    QString httpMethodString() const;
    void setHttpMethod(HttpMethod method);
    bool setHttpMethod(const QString &newMethod);

    bool invokeMethod(const QByteArray &requestData = QByteArray());
    QVariant replyReadParsed();
    QByteArray replyReadRaw();
    QString replyRead();

    QString errorInfo() const;
    bool isErrorState() const;
    bool isReplyReady() const;

signals:
    void replyReady(const QByteArray &rply);
    void errorEncountered(const QString &errMessage);

    // For QObject properties:
    void hostChanged();
    void hostUrlChanged();
    void nameChanged();
    void targetNamespaceChanged();
    void parameterNamesChanged();
    void protocolChanged();
    void httpMethodChanged();

protected slots:
    void replyFinished(QNetworkReply *reply);
    void authReplyFinished(QNetworkReply *reply);
    void authenticationSlot(QNetworkReply *reply, QAuthenticator *authenticator);

protected:
    QWebMethod(QWebMethodPrivate &d,
               Protocol protocol = Soap12, HttpMethod httpMethod = Post,
               QObject *parent = 0);
    QWebMethodPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QWebMethod)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QWebMethod::Protocols)

#endif // QWEBMETHOD_H
