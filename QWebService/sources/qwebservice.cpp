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

#include "../headers/qwebservice.h"

/*!
    \class QWebService
    \brief Parent Web Service class.

    Currently acts mostly as QWsdl wrapper. Some functionality might be pushed to
    QWebServiceReader in the future, to make this class more general.
  */

/*!
    Constructs the object with empty QWsdl and using \a parent (defaults to 0).
    You can set a WSDL file later using setWsdl(), or add custom web methods
    using addMethod().

    \sa setWsdl(), addMethod()
  */
QWebService::QWebService(QObject *parent)
    : QObject(parent)
{
    wsdl = new QWsdl(this);
    methods = new QMap<QString, QWebServiceMethod *>();
    init();
}

/*!
    Constructs and initializes the object with \a _wsdl and
    using \a parent (defaults to 0).
    WSDL file can be resetted later using resetWsdl().
    You can add custom web methods using addMethod().

    \sa resetWsdl(), addMethod()
  */
QWebService::QWebService(QWsdl *_wsdl, QObject *parent)
    : QObject(parent)
{
    wsdl = _wsdl;
    methods = new QMap<QString, QWebServiceMethod *>();
    init();
    methods = wsdl->methods();
}

/*!
    Constructs and initializes the object using \a parent (defaults to 0).
    \a _hostname is used to initialise QWsdl if an URL or path to
    a valid WSDL file was used. Otherwise, WSDL can be set later with setWsdl(),
    or add custom web methods
    using addMethod().

    \sa setWsdl(), addMethod()
  */
QWebService::QWebService(const QString &_hostname, QObject *parent)
    : QObject(parent)
{
    m_hostUrl.setUrl(_hostname);
    wsdl = new QWsdl(_hostname, this);
    methods = new QMap<QString, QWebServiceMethod *>();
    init();

    if (!wsdl->isErrorState())
        methods = wsdl->methods();
}

/*!
    Deletes wsdl pointer.
  */
QWebService::~QWebService()
{
    delete wsdl;
}

/*!
    \fn QWebService::errorEncountered(const QString &errMessage)

    Singal emitted when WsdlConverter encounters an error.
    Carries \a errMessage for convenience.
  */

/*!
    Returns a list of methods' names.
  */
QStringList QWebService::methodNames() const
{
    return (QStringList) methods->keys();
}

/*!
    Returns list of names of parameters of a method specified by \a methodName.
  */
QStringList QWebService::methodParameters(const QString &methodName) const
{
    return methods->value(methodName)->parameterNames();
}

/*!
    Returns names of return values, for a given method (\a methodName).
  */
QStringList QWebService::methodReturnValue(const QString &methodName) const
{
    return methods->value(methodName)->returnValueName();
}

/*!
    For a given \a methodName, returns a QMap with QString paramater name
    as a key, and QVariant value. By running QVariant::typeName() you can
    determine the type of the parameter.
  */
QMap<QString, QVariant> QWebService::parameterNamesTypes(const QString &methodName) const
{
    return methods->value(methodName)->parameterNamesTypes();
}

/*!
    For a given \a methodName, returns a QMap with QString paramater name
    as a key, and QVariant value. By running QVariant::typeName() you can
    determine the type of the parameter.
  */
QMap<QString, QVariant> QWebService::returnValueNameType(const QString &methodName) const
{
    return methods->value(methodName)->returnValueNameType();
}

/*!
    Adds the specified web method (\a newMethod) to the web service.
    Method name should be set in the object, it is used internally and
    for referencing.

    \sa removeMethod()
  */
void QWebService::addMethod(QWebServiceMethod *newMethod)
{
    methods->insert(newMethod->methodName(), newMethod);
}

/*!
    \overload addMethod()

    Adds the specified web method (\a newMethod) to the web service.
    Method name (\a methodName) is used for reference (it does not set name
    inside the QWebServiceMethod object).

    \sa removeMethod()
  */
void QWebService::addMethod(const QString &methodName, QWebServiceMethod *newMethod)
{
//    newMethod->setMessageName(methodName);
    methods->insert(methodName, newMethod);
}

/*!
    Removes the method specified by \a methodName.
    It also deletes the underlying object, so be careful!

    \sa addMethod()
  */
void QWebService::removeMethod(const QString &methodName)
{
    delete methods->value(methodName);
    methods->remove(methodName);
}

/*!
    Sets new \a hostname URL, resets the WSDL, and reinitialises.
  */
void QWebService::setHost(const QString &hostname)
{
    m_hostUrl.setUrl(hostname);
    resetWsdl(new QWsdl(hostname, this));
    init();
}

/*!
    \overload setHost()

    Sets new \a hostUrl URL, resets the WSDL, and reinitialises.
  */
void QWebService::setHost(const QUrl &hostUrl)
{
    m_hostUrl = hostUrl;
    resetWsdl(new QWsdl(hostUrl.host(), this));
    init();
}

/*!
    Sets the WSDL (\a newWsdl) file to use. This does not override
    already present methods. If you want to override them, use resetWsdl().

    \sa resetWsdl()
  */
void QWebService::setWsdl(QWsdl *newWsdl)
{
//    delete wsdl;
    wsdl = newWsdl;
    foreach (QString s, wsdl->methods()->keys()) {
        methods->insert(s, wsdl->methods()->value(s));
    }
}

/*!
    Sets the WSDL (\a newWsdl) file to use. This does override already
    present methods.
    If you just want to add WSDL methods to existing ones, use setWsdl()

    \sa setWsdl()
  */
void QWebService::resetWsdl(QWsdl *newWsdl)
{
//    delete wsdl;

    if (newWsdl == 0) {
        methods->clear();
        wsdl = new QWsdl(this);
    } else {
        wsdl = newWsdl;
        methods->clear();
//        delete methods;
        methods = wsdl->methods();
    }
}

/*!
    Returns QUrl of the web service.
  */
QUrl QWebService::hostUrl() const
{
    return m_hostUrl;
}

/*!
    Returns QString with URL of the web service.
  */
QString QWebService::host() const
{
    return m_hostUrl.path();
}

/*!
    Returns true if object is in error state.
  */
bool QWebService::isErrorState()
{
    if (wsdl->isErrorState())
        errorState = true;
    return errorState;
}

/*!
    Returns QString with error message in case an error occured.
    Otherwise, returns empty string.

    \sa isErrorState()
  */
QString QWebService::errorInfo() const
{
    return errorMessage;
}

/*!
    \internal

    Initialises the object, resets error state.
  */
void QWebService::init()
{
    errorState = false;
    errorMessage = QString::fromLatin1("");

    if (isErrorState())
        return;
}

/*!
    \internal

    Enters into error state with message \a errMessage.
  */
bool QWebService::enterErrorState(const QString &errMessage)
{
    errorState = true;
    errorMessage += QString(errMessage + " ");
//    qDebug() << errMessage;
    emit errorEncountered(errMessage);
    return false;
}
