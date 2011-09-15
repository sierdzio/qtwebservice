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

#include "../headers/qwebservicemethod.h"

/*!
    \class QWebServiceMethod
    \brief Extends QWebMethod with some generic constructors and synchronous message sending.


    Subclass of QWebMethod, contains many generic methods for sending messages.
    Can be used both synchronously (through static sendMessage() method),
    or asynchronously (indicates, when reply is ready by emitting
    a replyReady() signal).

    More description will be added in the future.
  */

/*!
  Constructs web method object with \a parent.
  Requires specifying other parameters later.
  If you use that constructor, you can probably go on
  and use the base QWebMethod class.
  */
QWebServiceMethod::QWebServiceMethod(QObject *parent) :
    QWebMethod(parent)
{
}

/*!
    A constructor that takes in \a protocol information,
    \a httpMethod to use, and \a parent
    to satisfy QObject requirements. All other data has to
    be set using setter methods.
  */
QWebServiceMethod::QWebServiceMethod(Protocol protocol, HttpMethod httpMethod,
                                     QObject *parent) :
    QWebMethod(protocol, httpMethod, parent)
{
}

/*!
    Constructs the message using \a hostUrl, \a methodName, \a parent,
    \a protocol (which defaults to soap12),
    and \a method (which defaults to POST).
    Requires params to be specified later.

    \sa init(), setParameters(), setProtocol(), sendMessage()
  */
QWebServiceMethod::QWebServiceMethod(const QUrl &hostUrl, const QString &methodName,
                                     Protocol protocol, HttpMethod method,
                                     QObject *parent) :
    QWebMethod(protocol, method, parent)
{
    m_hostUrl = hostUrl;
    m_methodName = methodName;
    setProtocol(protocol);
    setHttpMethod(method);
}

/*!
    Constructs the message using \a host, \a methodName, \a parent,
    \a protocol (which defaults to soap12),
    and \a httpMethod (which defaults to POST).
    Requires params to be specified later.

    \sa init(), setParameters(), setProtocol(), sendMessage()
  */
QWebServiceMethod::QWebServiceMethod(const QString &host, const QString &methodName,
                                     Protocol protocol, HttpMethod httpMethod,
                                     QObject *parent) :
    QWebMethod(protocol, httpMethod, parent)
{
    m_methodName = methodName;
    setProtocol(protocol);
    setHttpMethod(httpMethod);
    m_hostUrl.setUrl(host);
}

/*!
    Constructs the message using \a host, \a methodName, \a targetNamespace,
    \a parent, \a protocol (which defaults to soap12),
    and \a httpMethod (which defaults to POST).
    This constructor also takes message parameters (\a params).
    Does not require specifying any more information, but you still need to
    manually send the message using sendMessage() (without any arguments,
    or else - if you want to change ones specified here).

    \sa init(), sendMessage(), setProtocol()
  */
QWebServiceMethod::QWebServiceMethod(const QString &host, const QString &methodName,
                                     const QString &targetNamespace,
                                     const QMap<QString, QVariant> &params,
                                     Protocol protocol, HttpMethod httpMethod,
                                     QObject *parent) :
    QWebMethod(protocol, httpMethod, parent)
{
    m_methodName = methodName;
    m_targetNamespace = targetNamespace;
    parameters = params;
    setProtocol(protocol);
    setHttpMethod(httpMethod);
    m_hostUrl.setUrl(host);
}


/*!
    \overload sendMessage()

    Sends the message asynchronously using parameters specified in \a params.

    Returns true on success.
  */
bool QWebServiceMethod::sendMessage(const QMap<QString, QVariant> &params)
{
    parameters = params;
    sendMessage();
    return true;
}

/*!
    \overload sendMessage()

    STATIC method. Sends the message synchronously, using \a url, \a msgName,
    \a tNamespace, \a params and \a parent.
    Protocol can optionally be specified by \a protocol (default is SOAP 1.2),
    as well as HTTP \a method (default is POST).
    Returns with web service reply.
  */
QByteArray QWebServiceMethod::sendMessage(const QUrl &url,
                                          const QString &methodName,
                                          const QString &targetNamespace,
                                          const QMap<QString, QVariant> &params,
                                          Protocol protocol, HttpMethod httpMethod,
                                          QObject *parent)
{
    QWebServiceMethod qsm(url.toString(), methodName, targetNamespace, params,
                          protocol, httpMethod, parent);

    qsm.sendMessage();
    // TODO: ADD ERROR HANDLING!
    forever {
        if (qsm.replyReceived) {
            return qsm.reply;
        } else {
            qApp->processEvents();
        }
    }
}
