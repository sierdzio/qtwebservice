/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the QWebService library, QtNetwork Module.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
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
#include "QWebService_global.h"

class QWebMethodPrivate;

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
    QWebMethod(const QUrl &url, Protocol protocol = Soap12, HttpMethod httpMethod = Post,
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
    void setProtocol(Protocol protocol);

    HttpMethod httpMethod() const;
    QString httpMethodString() const;
    void setHttpMethod(HttpMethod method);
    bool setHttpMethod(const QString &newMethod);

    // should this be changed to invokeMethod()?
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

protected slots:
    void replyFinished(QNetworkReply *reply);
    void authReplyFinished(QNetworkReply *reply);
    void authenticationSlot(QNetworkReply *reply, QAuthenticator *authenticator);

protected:
    QWebMethod(QWebMethodPrivate &d,
            Protocol protocol = Soap12, HttpMethod httpMethod = Post,
            QObject *parent = 0);

private:
    Q_DECLARE_PRIVATE(QWebMethod)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QWebMethod::Protocols)

#endif // QWEBMETHOD_H
