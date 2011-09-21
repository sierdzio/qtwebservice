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

#ifndef QWEBSERVICEMETHOD_H
#define QWEBSERVICEMETHOD_H

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include "QWebService_global.h"
#include "qwebmethod.h"

class QWebServiceMethodPrivate;

class QWEBSERVICESHARED_EXPORT QWebServiceMethod : public QWebMethod
{
    Q_OBJECT

public:
    explicit QWebServiceMethod(QObject *parent = 0);
    QWebServiceMethod(Protocol protocol = Soap12, HttpMethod httpMethod = Post,
                               QObject *parent = 0);
    QWebServiceMethod(const QUrl &hostUrl, const QString &methodName,
                      Protocol protocol = Soap12, HttpMethod httpMethod = Post,
                      QObject *parent = 0);
    QWebServiceMethod(const QString &host, const QString &methodName,
                      Protocol protocol = Soap12, HttpMethod httpMethod = Post,
                      QObject *parent = 0);
    QWebServiceMethod(const QString &host, const QString &methodName,
                      const QString &targetNamespace,
                      const QMap<QString, QVariant> &params,
                      Protocol protocol = Soap12, HttpMethod httpMethod = Post,
                      QObject *parent = 0);

    using QWebMethod::sendMessage;
    bool sendMessage(const QMap<QString, QVariant> &params);
    QByteArray static sendMessage(const QUrl &url,
                                  const QString &methodName,
                                  const QString &targetNamespace,
                                  const QMap<QString, QVariant> &params,
                                  Protocol protocol = Soap12,
                                  HttpMethod httpMethod = Post,
                                  QObject *parent = 0);

protected:
    QWebServiceMethod(QWebServiceMethodPrivate &d,
                      Protocol protocol = Soap12, HttpMethod httpMethod = Post,
                      QObject *parent = 0);

private:
    Q_DECLARE_PRIVATE(QWebServiceMethod)
};

#endif // QWEBSERVICEMETHOD_H
