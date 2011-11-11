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

#include "../headers/qwebservice_p.h"

/*!
    \class QWebService
    \brief Class providing Web Service functionality.

    This class serves as central point for use cases where multiple web methods are needed,
    that share certain similarities.
    You can store all relevant information about a web service in a single class.

    You can supply a WSDl file, which will be parsed (using QWsdl), and web
    methods will be extracted. You can also specify your own web methods by
    using addMethod().

    You can use QWebService to gather data about web service (getName(),
    getHost(), getMethodNames() etc.), or use it's web methods to interact with
    remote web service (methods(), method()).

    Especially useful is the method(), which returns a pointer to a single web
    method, which  can be used directly to interact with QWebMethod object. For example:
    \code
        QWebService myWebService;
        ...
        myWebService.method("myWebMethod").invokeMethod();
    \endcode

    For convenience, QWebService::invokeMethod() and QWebService::replyRead() can also be used.

    When any of the web methods in QwebService receives a reply, replyReady() signal
    is emitted. It sends reply data and web method name, so that the sender can be easily
    determined.
  */

/*!
    \property QWebService::name
    \brief Holds web service name

    This property's default is empty string.
*/
/*!
    \property QWebService::host
    \brief Holds web service host Url as QString

    This property's default is empty string.
*/
/*!
    \property QWebService::hostUrl
    \brief Holds web service url as QUrl

    This property's default is empty QUrl.
*/
/*!
    \property QWebService::methodNames
    \brief Holds names of methods available in this web service

    This property's default is empty QStringList.
*/

/*!
    Constructs the object using \a parent (defaults to 0).
    You can set a WSDL file later using setWsdl(), or add custom web methods
    using addMethod().

    \sa setWsdl(), addMethod()
  */
QWebService::QWebService(QObject *parent)
    : QObject(parent), d_ptr(new QWebServicePrivate)
{
    Q_D(QWebService);
    d->wsdl = new QWsdl(this);
    d->methods = new QMap<QString, QWebMethod *>();
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
    : QObject(parent), d_ptr(new QWebServicePrivate)
{
    Q_D(QWebService);
    d->methods = new QMap<QString, QWebMethod *>();
    setWsdl(_wsdl);
    d->init();
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
    : QObject(parent), d_ptr(new QWebServicePrivate)
{
    Q_D(QWebService);
    d->m_hostUrl.setUrl(_hostname);
    d->methods = new QMap<QString, QWebMethod *>();
    setWsdl(new QWsdl(_hostname, this));
    d->init();

    if (!d->wsdl->isErrorState()) {
        setName(d->wsdl->webServiceName());
    }
}

/*!
    \internal

    Internal constuctor needed by private headers implementation.
  */
QWebService::QWebService(QWebServicePrivate &dd, QObject *parent) :
    QObject(parent), d_ptr(&dd)
{
    Q_D(QWebService);
    d->wsdl = new QWsdl(this);
    d->methods = new QMap<QString, QWebMethod *>();
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
    \fn QWebService::replyReady(const QByteArray &reply, const QString &methodName)

    Signal emitted when any of QWebMethods receives a \a reply. The web method
    that got the reply can be determined using \a methodName.
  */

/*!
    Returns name of the web service, as taken from QWsdl
    or set by setName().

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
    emit nameChanged();
}

/*!
    Returns pointers to all QWebMethod objects held in QWebService,
    useful for invoking web methods.

    \sa method()
  */
QMap<QString, QWebMethod *> *QWebService::methods()
{
    Q_D(QWebService);
    return d->methods;
}

/*!
    Returns a pointer to a single web method object, specified by
    \a methodName. If no method with that name exists,
    a default-constructed value is returned (see QMap::value() documentation
    for details).

    \sa methods()
  */
QWebMethod *QWebService::method(const QString &methodName)
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
void QWebService::addMethod(QWebMethod *newMethod)
{
    Q_D(QWebService);
    d->methods->insert(newMethod->methodName(), newMethod);
    connect(newMethod, SIGNAL(replyReady(QByteArray)),
            this, SLOT(receiveReply(QByteArray)));
    emit methodNamesChanged();
}

/*!
    \overload addMethod()

    Adds the specified web method (\a newMethod) to the web service.
    Method name (\a methodName) is used for reference (it does not set name
    inside the QWebMethod object).

    \sa removeMethod()
  */
void QWebService::addMethod(const QString &methodName, QWebMethod *newMethod)
{
    Q_D(QWebService);
    d->methods->insert(methodName, newMethod);
    connect(newMethod, SIGNAL(replyReady(QByteArray)),
            this, SLOT(receiveReply(QByteArray)));
    emit methodNamesChanged();
}

/*!
    Removes the method specified by \a methodName.
    It also deletes the underlying object, so be careful!

    \sa addMethod()
  */
void QWebService::removeMethod(const QString &methodName)
{
    Q_D(QWebService);
    disconnect(d->methods->value(methodName), SIGNAL(replyReady(QByteArray)),
            this, SLOT(receiveReply(QByteArray)));
    delete d->methods->value(methodName);
    d->methods->remove(methodName);
    emit methodNamesChanged();
}

/*!
    Invokes a web method, specified by given \a methodName. Passes \a data to the method.
    If no data specified, invokes (empty method body - common for getter methods).
    Returns true on success.

    Similar to calling:
    \code
    QWebService::method("methodName")->invokeMethod(data);
    \endcode
  */
bool QWebService::invokeMethod(const QString &methodName, const QByteArray &data)
{
    Q_D(QWebService);
    return d->methods->value(methodName)->invokeMethod(data);
}

/*!
    Read the reply of a web method, specified by given \a methodName.
    Returns empty string when no reply is present. See also replyReady()
    signal, as it can be used to determine, when a reply can be read.

    Similar to calling:
    \code
    QWebService::method("methodName")->replyRead();
    \endcode
  */
QString QWebService::replyRead(const QString &methodName)
{
    Q_D(QWebService);
    return d->methods->value(methodName)->replyRead();
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
    emit hostChanged();
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
    emit hostUrlChanged();
}

/*!
    Sets the WSDL (\a newWsdl) file to use. This does not override
    already present methods. If you want to override them, use resetWsdl().

    \sa resetWsdl()
  */
void QWebService::setWsdl(QWsdl *newWsdl)
{
    Q_D(QWebService);
    d->wsdl = newWsdl;
    setName(d->wsdl->webServiceName());
    foreach (QString s, d->wsdl->methods()->keys()) {
        d->methods->insert(s, d->wsdl->methods()->value(s));
        connect(d->methods->value(s), SIGNAL(replyReady(QByteArray)),
                this, SLOT(receiveReply(QByteArray)));
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

    if (newWsdl == 0) {
        foreach (QString s, d->methods->keys()) {
            connect(d->methods->value(s), SIGNAL(replyReady(QByteArray)),
                    this, SLOT(receiveReply(QByteArray)));
        }
        d->methods->clear();
        d->wsdl = new QWsdl(this);
        setName();
    } else {
        d->wsdl = newWsdl;
        d->methods->clear();
//        d->methods = d->wsdl->methods();
        foreach (QString s, d->wsdl->methods()->keys()) {
            d->methods->insert(s, d->wsdl->methods()->value(s));
            connect(d->methods->value(s), SIGNAL(replyReady(QByteArray)),
                    this, SLOT(receiveReply(QByteArray)));
        }
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
    emit q->errorEncountered(errMessage);
    return false;
}

/*!
    \internal

    Emits proper replyReady() signal after a web method reports that
    a reply is ready.
  */
void QWebService::receiveReply(const QByteArray &reply)
{
    QWebMethod *sendingMethod = qobject_cast<QWebMethod *>(QObject::sender());
    QString sendingMethodName = sendingMethod->methodName();
    emit replyReady(reply, sendingMethodName);
}
