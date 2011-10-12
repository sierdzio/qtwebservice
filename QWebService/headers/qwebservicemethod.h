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

    using QWebMethod::invokeMethod;
    bool invokeMethod(const QMap<QString, QVariant> &params);
    QByteArray static invokeMethod(const QUrl &url,
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
