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

#include "../headers/qwebmethod_p.h"

/*!
    \class QWebMethod
    \brief Class that can be used to asnchronously and synchronously send HTTP,
           SOAP 1.0, SOAP 1.2, JSON, XMLand REST messages to web services.

    QWebMethod class is acting as web method. You can use it to send and retrieve
    data from web services. There are many helper classes. QWsdl can be used to read
    a WSDL file - it will automatically create a QMap of QWebMethods,
    all already set up and ready to be invoked. QWebService can be used
    similarily as it is a high-level wrapper to QWsdl. Additionally,
    qtWsdlConverter console application can automatically generate custom classes
    for any WSDL file.

    If you need to authenticate on the server, use authenticate() method.
    You can specify username and password right there, or before,
    using setCredentials(), setUsername() and/ or setPassword().

    Typically, to send a message, you will need to set the URL, message name,
    target namespace (when using SOAP), parameters list (when invoking a medhod
    that has parameters), and then use the parameterless sendMessage() method.
    Here is a snippet for that:
    \code
    QWebMethod *message = new QWebMethod(0, QWebMethod::soap12, QWebMethod::POST);
    message->setHost("http://www.currencyserver.de/webservice/currencyserverwebservice.asmx");
    message->setMessageName("getProviderList");
    message->setTargetNamespace("http://www.daenet.de/webservices/CurrencyServer");
    message->sendMessage();
    \endcode
    You then have to wait for replyReady(QVariant) signal, or check for reply
    using isReplyReady() convenience method.

    To send a REST message with (for example) JSON body, pass
    (QWebMethod::rest | QWebMethod::json) as protocol flag. Additionally,
    specify HTTP method to be used (POST, GET, PUT, DELETE).
    When sending a REST message, \a messageName is used as request URI,
    and \a parameters specify additioanl data to be sent in message body.

    This class provides asynchronous sendMessage() only.
    If synchronous operation is needed, you can:
    \list
        \o use QWebServiceMethod, which provides some additional,
           useful constructors and methods
        \o use static QWebServiceMethod::sendMessage()
        \o subclass QWebMethod, and add a static sendMessage() method with
           a "waiting loop"
        \o wait for replyReady() signal in a "waiting loop"
    \endlist

    Here's a waiting loop snippet:
    \code
    QWebMethod qsm;
    ...
    qsm.sendMessage();
    forever {
        if (qsm.replyReceived) {
            return qsm.reply;
        } else {
            qApp->processEvents(); // Ensures that application remains responive
                                   // to events (prevents freezing).
        }
    }
    \endcode

    If you want to save some time on configuration in your code, you can
    subclass QWebMethod, implement a method, that would store your settings
    (an example configure() method is described below). Then, create your own
    slot for parsing the reply.

    In your "settings method", named for example configure(), you may want to:
    \list
        \o set host Url
        \o set protocol
        \o set message name
        \o set target namespace
        \o connect replyReady() signal to your custom slot (where you can
           parse the reply and return it in a type of your convenience)
    \endlist

    \sa init()
  */

/*!
     \enum QWebMethod::Protocol

     This enum type specifies the protocol that QWebMethod will use in
     communicating with the server:

     \value Http
            HTTP protocol will be used.
     \value Soap10
            SOAP 1.0 will be used.
     \value Soap12
            SOAP 1.2 will be used.
     \value Soap
            Convenience SOAP aggregator, used internally. If passed to
            setProtocol(), SOAP 1.2 will be used instead.
     \value Json
            JSON message will be used.
     \value Xml
            Message will be sent using plain XML (much simpler than SOAP).
     \value Rest
            QWebMethod will switch into REST mode. This can be combined with
            any other Protocol flag to define, what message body should look like.
 */

/*!
    \enum QWebMethod::HttpMethod

    Defines HTTP method to use when sending the message. Using more than 1 at
    the same time is forbidden (will not work).

    \value Post
           QWebMethod will use POST.
    \value Get
           QWebMethod will use GET.
    \value Put
           QWebMethod will use PUT.
    \value Delete
           QWebMethod will use DELETE.
  */

/*!
  Constructs web method object with \a parent.
  Requires specifying other parameters later.

  \sa init(), setParameters(), setProtocol(), sendMessage()
  */
QWebMethod::QWebMethod(QObject *parent) :
    QObject(parent), d_ptr(new QWebMethodPrivate)
{
    Q_D(QWebMethod);
    d->init();
    setProtocol(Soap12);
    setHttpMethod(Post);
}

/*!
    Constructs web method usign \a parent, \a protocol (which defaults to
    soap12), and \a method (which defaults to POST). Requires specifying
    other params later (setParameters()).

    \sa init(), setParameters(), setProtocol(), sendMessage()
  */
QWebMethod::QWebMethod(Protocol protocol, HttpMethod method, QObject *parent) :
    QObject(parent), d_ptr(new QWebMethodPrivate)
{
    Q_D(QWebMethod);
    d->init();
    setProtocol(protocol);
    setHttpMethod(method);
}

/*!
    Constructs web method usign \a parent, \a protocol (which defaults to soap12),
    and \a method (which defaults to POST), and \a url. Especially convenient for
    parameterless methods (like quick GET messages).

    \sa init(), setParameters(), setProtocol(), sendMessage()
  */
QWebMethod::QWebMethod(const QUrl &url, Protocol protocol, HttpMethod method, QObject *parent) :
    QObject(parent), d_ptr(new QWebMethodPrivate)
{
    Q_D(QWebMethod);
    d->init();
    setProtocol(protocol);
    setHttpMethod(method);
    d->m_hostUrl = url;
}

/*!
  \internal
  */
QWebMethod::QWebMethod(QWebMethodPrivate &dd, Protocol protocol, HttpMethod httpMethod, QObject *parent) :
    QObject(parent), d_ptr(&dd)
{
    Q_D(QWebMethod);
    d->init();
    setProtocol(protocol);
    setHttpMethod(httpMethod);
}

/*!
    Deletes internal pointers.
  */
QWebMethod::~QWebMethod()
{
    Q_D(QWebMethod);
    delete d->manager;
}

/*!
    \fn QWebMethod::errorEncountered(const QString &errMessage)

    Singal emitted when WsdlConverter encounters an error.
    Carries \a errMessage for convenience.
  */

/*!
    Sets message's host to \a newHost.
  */
void QWebMethod::setHost(const QString &newHost)
{
    Q_D(QWebMethod);
    d->m_hostUrl.setPath(newHost);
}

/*!
    Sets message's host to \a newHost.
  */
void QWebMethod::setHost(const QUrl &newHost)
{
    Q_D(QWebMethod);
    d->m_hostUrl = newHost;
}

/*!
    Used for authentication. Sets username using \a newUsername.
    To authenticate, call authenticate().

    \sa setPassword(), setCredentials(), authenticate()
  */
void QWebMethod::setUsername(const QString &newUsername)
{
    Q_D(QWebMethod);
    d->m_username = newUsername;
}

/*!
    Used for authentication. Sets password using \a newPassword.
    To authenticate, call authenticate().

    \sa setUsername(), setCredentials(), authenticate()
  */
void QWebMethod::setPassword(const QString &newPassword)
{
    Q_D(QWebMethod);
    d->m_password = newPassword;
}

/*!
    Used for authentication. Sets username (\a newUsername) and password
    (\a newPassword). To authenticate, call authenticate().

    \sa setUsername(), setPassword(), authenticate()
  */
void QWebMethod::setCredentials(const QString &newUsername, const QString &newPassword)
{
    Q_D(QWebMethod);
    d->m_username = newUsername;
    d->m_password = newPassword;
}

/*!
    Sets message's name to \a newName.
  */
void QWebMethod::setMessageName(const QString &newName)
{
    Q_D(QWebMethod);
    d->m_methodName = newName;
}

/*!
    Sets message's name to \a newName.
  */
void QWebMethod::setMethodName(const QString &newName)
{
    Q_D(QWebMethod);
    d->m_methodName = newName;
}

/*!
    Sets method's parameters (\a params). This also includes their names.
  */
void QWebMethod::setParameters(const QMap<QString, QVariant> &params)
{
    Q_D(QWebMethod);
    d->parameters = params;
}

/*!
    Sets method's return value (\a returnVal). This also includes their names.
  */
void QWebMethod::setReturnValue(const QMap<QString, QVariant> &returnVal)
{
    Q_D(QWebMethod);
    d->returnValue = returnVal;
}

/*!
    Sets message's target namespace (\a tNamespace),
    which is needed in SOAP messaging.
  */
void QWebMethod::setTargetNamespace(const QString &tNamespace)
{
    Q_D(QWebMethod);
    d->m_targetNamespace = tNamespace;
}

/*!
    Sets the protocol flag (\a prot, being one of the values
    of QWebMethod::Protocol). This determines the protocol used later,
    when sending request. Defaults to SOAP1.2.

    WARNING:
    This method also checks for disallowed combinations. If such combination
    is encountered, it discards the set protocol, and sets SOAP 1.2.

    This behavious will probably change in the future, when QWebMethod gets
    error handling.
  */
void QWebMethod::setProtocol(Protocol prot)
{
    Q_D(QWebMethod);
    // Prevent incompatibile flags from being set simultaneously:
    QList<int> allowedCombinations;
    // Standard values.
    allowedCombinations << 0x01 << 0x02 << 0x04 << 0x06 << 0x08 << 0x10 << 0x20;
    // REST combinations
    allowedCombinations << 0x21 << 0x22 << 0x24 << 0x26 << 0x28 << 0x30;

    if (allowedCombinations.contains(prot)) {
        if (prot & Soap)
            d->protocolUsed = Soap12;
        else
            d->protocolUsed = prot;
    } else {
        d->enterErrorState(QLatin1String("Wrong protocol is set. You have "
                                            "combined exclusive flags."));
    }
}

/*!
    Sets the httpMethod flag (\a method, a value of QWebMethod::HttpMethod).
    Default method is QWebMethod::POST.
  */
void QWebMethod::setHttpMethod(HttpMethod method)
{
    Q_D(QWebMethod);
    d->httpMethodUsed = method;
}

/*!
    Sets the httpMethod flag (\a newMethod, using QString representation
    of HTTP method (post, get, put, or delete)). Setting is NOT case sensitive.
    Default method is POST.

    Returns true if successful.
  */
bool QWebMethod::setHttpMethod(const QString &newMethod)
{
    Q_D(QWebMethod);
    QString tempMethod = newMethod.toLower();
    if (tempMethod == QLatin1String("post"))
        d->httpMethodUsed = Post;
    else if (tempMethod == QLatin1String("get"))
        d->httpMethodUsed = Get;
    else if (tempMethod == QLatin1String("put"))
        d->httpMethodUsed = Put;
    else if (tempMethod == QLatin1String("delete"))
        d->httpMethodUsed = Delete;
    else
        return false;

    return true;
}

/*!
    Sends the message asynchronously, assuming that all neccessary data was
    specified earlier. Optionally, a QByteArray (\a requestData) can be
    specified - it will override standard data encapsulation (preparation,
    see prepareRequestData()), and send the byte array without any changes.

    If synchronous operation is needed, you can:
    \list
        \o use static QWebServiceMethod::sendMessage()
        \o subclass QWebMethod, and add a static sendMessage() method with
           a "waiting loop"
        \o wait for replyReady() signal in a "waiting loop"
    \endlist

    Here's a waiting loop snippet:
    \code
    QWebMethod qsm;
    ...
    qsm.sendMessage();
    forever {
        if (qsm.replyReceived) {
            return qsm.reply;
        } else {
            // Ensures that application remains responive to events (prevents freezing).
            qApp->processEvents();
        }
    }
    \endcode

    Returns true on success.

    \sa setParameters(), setProtocol(), setTargetNamespace(), prepareRequestData()
  */
bool QWebMethod::sendMessage(const QByteArray &requestData)
{
    Q_D(QWebMethod);
    connect(d->manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this, SLOT(authenticationSlot(QNetworkReply*,QAuthenticator*)));

    if ((d->authenticationPerformed == true)
            && (d->authenticationReplyReceived == false)) {
        forever {
            if (d->authenticationReplyReceived) {
                disconnect(d->manager, SIGNAL(finished(QNetworkReply*)),
                           this, SLOT(authReplyFinished(QNetworkReply*)));
                break;
            } else {
                qApp->processEvents();
            }
        }
    } else if ((d->authenticationPerformed == true)
               && (d->authenticationReplyReceived == true)) {
        disconnect(d->manager, SIGNAL(finished(QNetworkReply*)),
                   this, SLOT(authReplyFinished(QNetworkReply*)));
    }

    connect(d->manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    QNetworkRequest request;
    request.setUrl(d->m_hostUrl);

    if (d->protocolUsed & Soap) {
        request.setHeader(QNetworkRequest::ContentTypeHeader,
                          QVariant(QLatin1String("application/soap+xml; charset=utf-8")));
    } else if (d->protocolUsed & Json) {
        request.setHeader(QNetworkRequest::ContentTypeHeader,
                          QVariant(QLatin1String("application/json; charset=utf-8")));
    } else if (d->protocolUsed & Http) {
        request.setHeader(QNetworkRequest::ContentTypeHeader,
                          QVariant(QLatin1String("Content-Type: application/x-www-form-urlencoded")));
    } else if (d->protocolUsed & Xml) {
        request.setHeader(QNetworkRequest::ContentTypeHeader,
                          QVariant(QLatin1String("application/xml; charset=utf-8")));
    }

    if (d->protocolUsed & Soap10)
        request.setRawHeader(QByteArray("SOAPAction"),
                             QByteArray(d->m_hostUrl.toString().toAscii()));

    if (requestData.isNull() || requestData.isEmpty())
        d->prepareRequestData();
    else
        d->data = requestData;

    // OPTIONAL - FOR TESTING:
//    qDebug() << request.url().toString();
//    qDebug() << QString(d->data);
    // ENDOF: OPTIONAL - FOR TESTING

    if (d->protocolUsed & Rest) {
        if (d->httpMethodUsed == Post)
            d->manager->post(request, d->data);
        else if (d->httpMethodUsed == Get)
            d->manager->get(request);
        else if (d->httpMethodUsed == Put)
            d->manager->put(request, d->data);
        else if (d->httpMethodUsed == Delete)
            d->manager->deleteResource(request);
    } else {
        d->manager->post(request, d->data);
    }

    return true;
}

/*!
    Performs authentication using \a newUsername and \a newPassword,
    if specified. If not, and they were given using setCredentials(),
    setUsername() or setPassword(), it uses the existing values.

    If no data is specified, it does nothing. Returns true on success.

    \sa setCredentials(), setUsername(), setPassword()
  */
bool QWebMethod::authenticate(const QString &newUsername, const QString &newPassword)
{
    Q_D(QWebMethod);
    if (!newUsername.isNull())
        d->m_username = newUsername;
    if (!newPassword.isNull())
        d->m_password = newPassword;

    if (d->m_username != QLatin1String("")) {
        QUrl url;
        url.addEncodedQueryItem("ACT", QUrl::toPercentEncoding("11"));
        url.addEncodedQueryItem("RET", QUrl::toPercentEncoding("/"));
        url.addEncodedQueryItem("site_id", QUrl::toPercentEncoding("1"));
        url.addEncodedQueryItem("username", QUrl::toPercentEncoding(d->m_username));
        url.addEncodedQueryItem("password", QUrl::toPercentEncoding(d->m_password));

        return authenticate(url);
    }
    return false;
}

/*!
    Performs authentication using \a customAuthString.
    Credentials specified by setCredentials(),
    setUsername() or setPassword() are NOT used.

    If no data is specified, it does nothing (and returns false).
    Returns true on success.

    \sa setCredentials(), setUsername(), setPassword()
  */
bool QWebMethod::authenticate(const QUrl &customAuthString)
{
    Q_D(QWebMethod);
    if (customAuthString.isEmpty())
        return false;

    d->authenticationPerformed = true;
    d->authenticationReplyReceived = false;
    connect(d->manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(authReplyFinished(QNetworkReply*)));
    connect(d->manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this, SLOT(authenticationSlot(QNetworkReply*,QAuthenticator*)));

    QNetworkRequest rqst(QUrl::fromUserInput(
                             QString("http://" + d->m_hostUrl.host() + "/")));
    rqst.setHeader(QNetworkRequest::ContentTypeHeader,
                   QLatin1String("application/x-www-form-urlencoded"));

    QByteArray paramBytes = customAuthString.toString().mid(1).toLatin1();
    paramBytes.replace("/", "%2F");

    d->manager->post(rqst, paramBytes);
    return true;
}

/*!
    After making asynchronous call, and getting the replyReady() signal,
    this method can be used to read the reply.
  */
QString QWebMethod::replyRead()
{
    Q_D(QWebMethod);
    QString replyString(d->reply);
    replyString = d->convertReplyToUtf(replyString);
    return replyString;
}

/*!
    After making asynchronous call, and getting the replyReady() signal,
    this method can be used to read the reply.

    Returns parsed data.
  */
QVariant QWebMethod::replyReadParsed()
{
    Q_D(QWebMethod);
    QVariant result;
    QByteArray replyBytes = d->reply;
    QString replyString = d->convertReplyToUtf(QString(replyBytes));

    // This section is SOAP-only and should be fixed for other protocols!
    // It's not done properly, anyway.
    // Should return type specified in replyValue.
    if (d->protocolUsed & Soap) {
        QString tempBegin = QString("<" + d->m_methodName + "Result>");
        int replyBeginIndex = replyString.indexOf(tempBegin, 0,
                                                  Qt::CaseSensitive);
        replyBeginIndex += tempBegin.length();

        QString tempFinish = QString("</" + d->m_methodName + "Result>");
        int replyFinishIndex = replyString.indexOf(tempFinish,
                                                   replyBeginIndex,
                                                   Qt::CaseSensitive);

        if (replyBeginIndex == -1)
            replyBytes = 0;
        if (replyFinishIndex == -1)
            replyFinishIndex = replyString.length();

        result = (QVariant) replyString.mid(replyBeginIndex,
                                            replyFinishIndex - replyBeginIndex);
    } else {
        result = replyString;
    }

    return replyBytes;
}

/*!
    After making asynchronous call, and getting the replyReady() signal,
    this method can be used to read the reply.

    Returns the raw data acquired from server.
  */
QByteArray QWebMethod::replyReadRaw()
{
    Q_D(QWebMethod);
    return d->reply;
}

/*!
    \fn QWebMethod::replyReady(const QByteArray &rply)

    Signal invoked when the reply (\a rply) from web service's server
    is ready for reading.
  */

/*!
    Returns message's name.
  */
QString QWebMethod::methodName() const
{
    Q_D(const QWebMethod);
    return d->m_methodName;
}

/*!
    Retrurns list of parameters' names.
  */
QStringList QWebMethod::parameterNames() const
{
    Q_D(const QWebMethod);
    return (QStringList) d->parameters.keys();
}

/*!
    Returns return value's name.
  */
QStringList QWebMethod::returnValueName() const
{
    Q_D(const QWebMethod);
    return (QStringList) d->returnValue.keys();
}

/*!
    Returns whole parameter information (name and type).
  */
QMap<QString, QVariant> QWebMethod::parameterNamesTypes() const
{
    Q_D(const QWebMethod);
    return d->parameters;
}

/*!
    Returns whole return value information (name and type).
  */
QMap<QString, QVariant> QWebMethod::returnValueNameType() const
{
    Q_D(const QWebMethod);
    return d->returnValue;
}

/*!
    Returns target namespace.
  */
QString QWebMethod::targetNamespace() const
{
    Q_D(const QWebMethod);
    return d->m_targetNamespace;
}

/*!
    Returns host's URL (in QString). If you want a QUrl, call getHostUrl(),
    or QUrl(QWebMethod::getHost());
  */
QString QWebMethod::host() const
{
    Q_D(const QWebMethod);
    return d->m_hostUrl.path();
}

/*!
    Returns host's URL. If you want a QString, call getHost() or getHostUrl().host();
  */
QUrl QWebMethod::hostUrl() const
{
    Q_D(const QWebMethod);
    return d->m_hostUrl;
}

/*!
    Returns username.
  */
QString QWebMethod::username() const
{
    Q_D(const QWebMethod);
    return d->m_username;
}

/*!
    Returns currently set protocol.

    \sa httpMethod()
  */
QWebMethod::Protocol QWebMethod::protocol() const
{
    Q_D(const QWebMethod);
    return d->protocolUsed;
}

/*!
    Returns currently set HTTP method.

    \sa protocol()
  */
QWebMethod::HttpMethod QWebMethod::httpMethod() const
{
    Q_D(const QWebMethod);
    return d->httpMethodUsed;
}

/*!
    Returns protocol used in form of a QString. If \a includeRest is
    true, and --rest flag was specified, it appends ",rest" to the result.
  */
QString QWebMethod::protocolString(bool includeRest) const
{
    Q_D(const QWebMethod);
    QString result;

    if (d->protocolUsed & Http)
        result = QLatin1String("http");
    else if (d->protocolUsed & Soap10)
        result = QLatin1String("soap10");
    else if (d->protocolUsed & Soap12)
        result = QLatin1String("soap12");
    else if (d->protocolUsed & Json)
        result = QLatin1String("json");
    else if (d->protocolUsed & Xml)
        result = QLatin1String("xml");

    if (includeRest && (d->protocolUsed & Rest))
        result += QLatin1String(",rest");

    return result;
}

/*!
    Returns the HTTP method used, in form of a QString.

    \sa httpMethod()
  */
QString QWebMethod::httpMethodString() const
{
    Q_D(const QWebMethod);
    QString result;

    if (d->httpMethodUsed == Post)
        result = QLatin1String("POST");
    else if (d->httpMethodUsed == Get)
        result = QLatin1String("GET");
    else if (d->httpMethodUsed == Put)
        result = QLatin1String("PUT");
    else if (d->httpMethodUsed == Delete)
        result = QLatin1String("DELETE");

    return result;
}

/*!
    Returns QString with error message in case an error occured. Otherwise,
    returns empty string.

    \sa isErrorState()
  */
QString QWebMethod::errorInfo() const
{
    Q_D(const QWebMethod);
    return d->errorMessage;
}

/*!
    Returns true if there was an error, false otherwise. Details about
    an error can be read with getErrorInfo().

    \sa errorInfo()
  */
bool QWebMethod::isErrorState() const
{
    Q_D(const QWebMethod);
    return d->errorState;
}

/*!
    Returns true if reply was received.
  */
bool QWebMethod::isReplyReady() const
{
    Q_D(const QWebMethod);
    return d->replyReceived;
}

/*!
    Private slot, which processes
    the reply (\a netReply) from the server.
    Emits the replyReady() signal.
  */
void QWebMethod::replyFinished(QNetworkReply *netReply)
{
    Q_D(QWebMethod);
    d->reply = netReply->readAll();
    d->replyReceived = true;
    emit replyReady(d->reply);
    netReply->deleteLater();
}

/*!
    TEMP Auth METHOD. HIGHLY EXPERIMENTAL.
    Checks for body of \a reply to determine correctness of authentication.
  */
void QWebMethod::authReplyFinished(QNetworkReply *reply)
{
    Q_D(QWebMethod);
    d->authenticationReplyReceived = true;
    QByteArray array = reply->readAll();
    if (!array.isEmpty())
    {
        d->enterErrorState(QLatin1String("Login incorrect."));
    }
    //    else        qDebug() << "Login correct";
    reply->deleteLater();
}

/*!
    Internal method used to authenticate the communication.
    Use setCredentials() or setUsername() and setPassword()
    to specify the data.

    This is a fallback method of QNAM. Typically, authenticate()
    should be used.

    Fills the \a authenticator object. Does not use \a reply.
  */
void QWebMethod::authenticationSlot(QNetworkReply *reply,
                                    QAuthenticator *authenticator)
{
    Q_D(QWebMethod);
    if (d->authenticationError)
    {
        d->enterErrorState(QString("Authentication error! " + reply->readAll()));
        return;
    }

    authenticator->setUser(d->m_username);
    authenticator->setPassword(d->m_password);
    d->authenticationError = true;
    reply->deleteLater();
}

/*!
    Performs genral initialisation of the object.
    Sets default variable values, initializes network manager,
    connects reply signals. Calls virtual method, configure().
  */
void QWebMethodPrivate::init()
{
    replyReceived = false;
    authenticationReplyReceived = false;
    errorState = false;
    authenticationError = false;
    authenticationPerformed = false;

    manager = new QNetworkAccessManager;
}

/*!
    Protected function, invoked by sendMessage(). Modifies QByteArray data,
    so that it is consistent with protocol and HTTP method specification.
    It uses QMap<QString, QVariant> parameters to fill data object's body.
    Can be overriden by creating custom QByteArray and passing it to
    sendMessage().

    \sa sendMessage()
  */
void QWebMethodPrivate::prepareRequestData()
{
    data.clear();
    QString header, body, footer;
    QString endl("\r\n"); // Replace with something OS-independent, or seriously rethink.

    if (protocolUsed & QWebMethod::Soap) {
        if (protocolUsed & QWebMethod::Soap12) {
            header = QString("<?xml version=\"1.0\" encoding=\"utf-8\"?> " + endl +
                     " <soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " +
                     "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" " +
                     "xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\"> " + endl +
                     " <soap12:Body> " + endl);

            footer = QString("</soap12:Body> " + endl + "</soap12:Envelope>");
        } else if (protocolUsed & QWebMethod::Soap10) {
            header = QString("<?xml version=\"1.0\" encoding=\"utf-8\"?> " + endl +
                    " <soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " +
                    "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" " +
                    "xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\"> " + endl +
                    " <soap:Body> " + endl);

            footer = QString("</soap:Body> " + endl + "</soap:Envelope>");
        }

        body = QString("\t<" + m_methodName
                       + " xmlns=\"" + m_targetNamespace + "\"> " + endl);

        foreach (const QString currentKey, parameters.keys()) {
            QVariant qv = parameters.value(currentKey);
            // Currently, this does not handle nested lists
            body += QString("\t\t<" + currentKey + ">" +
                            qv.toString() + "</" + currentKey + "> " + endl);
        }

        body += QString("\t</" + m_methodName + "> " + endl);
    } else if (protocolUsed & QWebMethod::Http) {
        foreach (const QString currentKey, parameters.keys()) {
            QVariant qv = parameters.value(currentKey);
            // Currently, this does not handle nested lists
            body += QString(currentKey + "=" + qv.toString() + "&");
        }
        body.chop(1);
    } else if (protocolUsed & QWebMethod::Json) {
        body += "{" + endl;
        foreach (const QString currentKey, parameters.keys()) {
            QVariant qv = parameters.value(currentKey);
            // Currently, this does not handle nested lists
            body += QString("{" + endl + "\t\"" + currentKey + "\" : \""
                            + qv.toString() + "\"" + endl);
        }
        body += QLatin1String("}");
    } else if (protocolUsed & QWebMethod::Xml) {
        foreach (const QString currentKey, parameters.keys()) {
            QVariant qv = parameters.value(currentKey);
            // Currently, this does not handle nested lists
            body += QString("\t\t<" + currentKey + ">"
                            + qv.toString() + "</" + currentKey + "> " + endl);
        }
    }

    data.append(QString(header + body + footer).toLatin1());
}

/*!
    \internal

    Changes the encoding of the reply, in a rather crude fashion.
  */
QString QWebMethodPrivate::convertReplyToUtf(const QString &textToConvert)
{
    QString result = textToConvert;

    result.replace(QLatin1String("&lt;"), QLatin1String("<"));
    result.replace(QLatin1String("&gt;"), QLatin1String(">"));

    return result;
}

/*!
    \internal

    Enters into error state with message \a errMessage.
  */
bool QWebMethodPrivate::enterErrorState(const QString &errMessage)
{
    Q_Q(QWebMethod);
    errorState = true;
    errorMessage += QString(errMessage + " ");
//    qDebug() << errMessage;
    emit q->errorEncountered(errMessage);
    return false;
}
