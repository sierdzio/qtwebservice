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

#ifndef QWEBMETHOD_P_H
#define QWEBMETHOD_P_H

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
#include "qwebmethod.h"

class QWebMethodPrivate
{
    Q_DECLARE_PUBLIC(QWebMethod)

public:
    QWebMethodPrivate() {}
    QWebMethodPrivate(QWebMethod *q) : q_ptr(q) {}
    QWebMethod *q_ptr;

    void init();
    void prepareRequestData();
    QString convertReplyToUtf(const QString &textToConvert);
    bool enterErrorState(const QString &errMessage = QString());

    bool errorState;
    bool authenticationPerformed;
    bool authenticationReplyReceived;
    bool authenticationError;
    QString errorMessage;
    bool replyReceived;
    QWebMethod::Protocol protocolUsed;
    QWebMethod::HttpMethod httpMethodUsed;
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

#endif // QWEBMETHOD_P_H
