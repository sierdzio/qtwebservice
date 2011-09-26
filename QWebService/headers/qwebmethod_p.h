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
#include <private/qobject_p.h>
#include "qwebmethod.h"

class QWebMethodPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QWebMethod)

public:
    QWebMethodPrivate() {}

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
