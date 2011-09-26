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

#include "../headers/qwebservice_p.h"

/*!
    \class QWebService
    \brief Class providing Web Service functionality.

    This class serves as central point for multi-web method uses. You can store
    all relevant information about a web service in a single class.

    You can supply a WSDl file, which will be parsed (using QWsdl), and web
    methods will be extracted. You can also specify your own web methods by
    using addMethod().

    You can use QWebService to gather data about web service (getName(),
    getHost(), getMethodNames() etc.), or use it's web methods to interact with
    remote web service (methods(), method()).

    Especially useful is the method(), which returns a pointer to a single web
    method, which  can be used directly to inreract with QWebMethod object.
  */

/*!
    Constructs the object with empty QWsdl and using \a parent (defaults to 0).
    You can set a WSDL file later using setWsdl(), or add custom web methods
    using addMethod().

    \sa setWsdl(), addMethod()
  */
QWebService::QWebService(QObject *parent)
    : QObject(*new QWebServicePrivate, parent)
{
    Q_D(QWebService);
    d->wsdl = new QWsdl(this);
    d->methods = new QMap<QString, QWebServiceMethod *>();
    d->init();
}

/*!
    Constructs and initializes the object with \a _wsdl and
    using \a parent (defaults to 0).
    WSDL file can be resetted later using resetWsdl().
    You can add custom web methods using addMethod().

    \sa resetWsdl(), addMethod()
  */
QWebService::QWebService(QWsdl *_wsdl, QObject *parent)
    : QObject(*new QWebServicePrivate, parent)
{
    Q_D(QWebService);
    d->wsdl = _wsdl;
    d->methods = new QMap<QString, QWebServiceMethod *>();
    d->init();
    d->methods = d->wsdl->methods();
    setName(d->wsdl->webServiceName());
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
    : QObject(*new QWebServicePrivate, parent)
{
    Q_D(QWebService);
    d->m_hostUrl.setUrl(_hostname);
    d->wsdl = new QWsdl(_hostname, this);
    d->methods = new QMap<QString, QWebServiceMethod *>();
    d->init();

    if (!d->wsdl->isErrorState()) {
        d->methods = d->wsdl->methods();
        setName(d->wsdl->webServiceName());
    }
}

/*!
  \internal
  */
QWebService::QWebService(QWebServicePrivate &dd, QObject *parent) :
    QObject(dd, parent)
{
    Q_D(QWebService);
    d->wsdl = new QWsdl(this);
    d->methods = new QMap<QString, QWebServiceMethod *>();
    d->init();
}

/*!
    Deletes wsdl pointer.
  */
QWebService::~QWebService()
{
    Q_D(QWebService);
    delete d->wsdl;
}

/*!
    \fn QWebService::errorEncountered(const QString &errMessage)

    Singal emitted when WsdlConverter encounters an error.
    Carries \a errMessage for convenience.
  */

/*!
    Returns name of the web service.

    \sa setName()
  */
QString QWebService::name() const
{
    Q_D(const QWebService);
    return d->webServiceName;
}

/*!
    Sets web service's name using \a newWebServiceName.
    This will override any name set before, including that
    extracted from WSDL file.

    \sa name()
  */
void QWebService::setName(const QString &newWebServiceName)
{
    Q_D(QWebService);
    d->webServiceName = newWebServiceName;
}

/*!
    Returns pointers to QWebMethod objects, useful for invoking web methods.

    \sa method()
  */
QMap<QString, QWebServiceMethod *> *QWebService::methods()
{
    Q_D(QWebService);
    return d->methods;
}

/*!
    Returns a pointer to a single web method object, sceficied by
    \a methodName. If now method with that name exists,
    a default-constructed value is returned (see QMap::value() documentation
    for details).

    \sa methods()
  */
QWebServiceMethod *QWebService::method(const QString &methodName)
{
    Q_D(QWebService);
    return d->methods->value(methodName);
}

/*!
    Returns a list of methods' names.
  */
QStringList QWebService::methodNames() const
{
    Q_D(const QWebService);
    return (QStringList) d->methods->keys();
}

/*!
    Returns list of names of parameters of a method specified by \a methodName.
  */
QStringList QWebService::methodParameters(const QString &methodName) const
{
    Q_D(const QWebService);
    return d->methods->value(methodName)->parameterNames();
}

/*!
    Returns names of return values, for a given method (\a methodName).
  */
QStringList QWebService::methodReturnValue(const QString &methodName) const
{
    Q_D(const QWebService);
    return d->methods->value(methodName)->returnValueName();
}

/*!
    For a given \a methodName, returns a QMap with QString paramater name
    as a key, and QVariant value. By running QVariant::typeName() you can
    determine the type of the parameter.
  */
QMap<QString, QVariant> QWebService::parameterNamesTypes(const QString &methodName) const
{
    Q_D(const QWebService);
    return d->methods->value(methodName)->parameterNamesTypes();
}

/*!
    For a given \a methodName, returns a QMap with QString paramater name
    as a key, and QVariant value. By running QVariant::typeName() you can
    determine the type of the parameter.
  */
QMap<QString, QVariant> QWebService::returnValueNameType(const QString &methodName) const
{
    Q_D(const QWebService);
    return d->methods->value(methodName)->returnValueNameType();
}

/*!
    Adds the specified web method (\a newMethod) to the web service.
    Method name should be set in the object, it is used internally and
    for referencing.

    \sa removeMethod()
  */
void QWebService::addMethod(QWebServiceMethod *newMethod)
{
    Q_D(QWebService);
    d->methods->insert(newMethod->methodName(), newMethod);
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
    Q_D(QWebService);
//    newMethod->setMessageName(methodName);
    d->methods->insert(methodName, newMethod);
}

/*!
    Removes the method specified by \a methodName.
    It also deletes the underlying object, so be careful!

    \sa addMethod()
  */
void QWebService::removeMethod(const QString &methodName)
{
    Q_D(QWebService);
    delete d->methods->value(methodName);
    d->methods->remove(methodName);
}

/*!
    Returns QString with URL of the web service.
  */
QString QWebService::host() const
{
    Q_D(const QWebService);
    return d->m_hostUrl.path();
}

/*!
    Returns QUrl of the web service.
  */
QUrl QWebService::hostUrl() const
{
    Q_D(const QWebService);
    return d->m_hostUrl;
}

/*!
    Sets new \a hostname URL, resets the WSDL, and reinitialises.
  */
void QWebService::setHost(const QString &hostname)
{
    Q_D(QWebService);
    d->m_hostUrl.setUrl(hostname);
    resetWsdl(new QWsdl(hostname, this));
    d->init();
}

/*!
    \overload setHost()

    Sets new \a hostUrl URL, resets the WSDL, and reinitialises.
  */
void QWebService::setHost(const QUrl &hostUrl)
{
    Q_D(QWebService);
    d->m_hostUrl = hostUrl;
    resetWsdl(new QWsdl(hostUrl.host(), this));
    d->init();
}

/*!
    Sets the WSDL (\a newWsdl) file to use. This does not override
    already present methods. If you want to override them, use resetWsdl().

    \sa resetWsdl()
  */
void QWebService::setWsdl(QWsdl *newWsdl)
{
    Q_D(QWebService);
//    delete wsdl;
    d->wsdl = newWsdl;
    setName(d->wsdl->webServiceName());
    foreach (QString s, d->wsdl->methods()->keys()) {
        d->methods->insert(s, d->wsdl->methods()->value(s));
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
    Q_D(QWebService);
//    delete wsdl;

    if (newWsdl == 0) {
        d->methods->clear();
        d->wsdl = new QWsdl(this);
        setName();
    } else {
        d->wsdl = newWsdl;
        d->methods->clear();
//        delete methods;
        d->methods = d->wsdl->methods();
        setName(d->wsdl->webServiceName());
    }
}

/*!
    Returns true if object is in error state.
  */
bool QWebService::isErrorState()
{
    Q_D(QWebService);
    if (d->wsdl->isErrorState())
        d->errorState = true;
    return d->errorState;
}

/*!
    Returns QString with error message in case an error occured.
    Otherwise, returns empty string.

    \sa isErrorState()
  */
QString QWebService::errorInfo() const
{
    Q_D(const QWebService);
    return d->errorMessage;
}

/*!
    \internal

    Initialises the object, resets error state.
  */
void QWebServicePrivate::init()
{
    errorState = false;

    if (wsdl->isErrorState())
        return;
}

/*!
    \internal

    Enters into error state with message \a errMessage.
  */
bool QWebServicePrivate::enterErrorState(const QString &errMessage)
{
    Q_Q(QWebService);
    errorState = true;
    errorMessage += QString(errMessage + QLatin1String(" "));
//    qDebug() << errMessage;
    emit q->errorEncountered(errMessage);
    return false;
}
