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
    target namespace (when using SOAP), parameters list (when invoking a method
    that has parameters), and then use the parameterless sendMessage() method.
    Here is a snippet for that:
    \code
    QWebMethod *message = new QWebMethod(0, QWebMethod::soap12, QWebMethod::POST);
    message->setHost("http://www.currencyserver.de/webservice/currencyserverwebservice.asmx");
    message->setMessageName("getProviderList");
    message->setTargetNamespace("http://www.daenet.de/webservices/CurrencyServer");
    message->invokeMethod();
    \endcode
    You then have to wait for replyReady(QVariant) signal, or check for reply
    using isReplyReady() convenience method.

    To send a REST message with (for example) JSON body, pass
    (QWebMethod::rest | QWebMethod::json) as protocol flag. Additionally,
    specify HTTP method to be used (POST, GET, PUT, DELETE).
    When invoking a REST method, \a methodName is used as request URI,
    and \a parameters specify additioanl data to be sent in message body.

    This class provides asynchronous sendMessage() only.
    If synchronous operation is needed, you can:
    \list
        \o use QWebServiceMethod, which provides some additional,
           useful constructors and methods, including a synchronous static
           sendMessage() method
        \o use static QWebServiceMethod::invokeMethod()
        \o subclass QWebMethod, and add a static invokeMethod() method with
           a "waiting loop"
        \o wait for replyReady() signal in a "waiting loop". You can use isReplyReady()
           for that
    \endlist

    Here's a waiting loop snippet:
    \code
    QWebMethod qsm;
    ...
    qsm.invokeMethod();
    forever {
        if (qsm.isReplyReady()) {
            return qsm.replyRead();
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
        \o set method name
        \o set target namespace
        \o connect replyReady() signal to your custom slot (where you can
           parse the reply and return it in a type of your convenience)
    \endlist
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

    Defines HTTP method to use when invoking the method. Using more than 1 at
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

/* //!
    \property QWebMethod::name
    \brief Holds method name

    This property's default is empty string.
*/
/* //!
    \property QWebMethod::host
    \brief Holds method's host Url as QString

    This property's default is empty string.
*/
/* //!
    \property QWebMethod::hostUrl
    \brief Holds method's host Url as QUrl

    This property's default is empty QUrl.
*/
/* //!
    \property QWebMethod::targetNamespace
    \brief Holds method's targetNamespace

    This property's default is empty string.
*/
/* //!
    \property QWebMethod::parameterNames
    \brief Holds names of parametrs of this web method

    This property's default is empty QStringList.
*/

/*!
    Constructs web method usign \a parent, \a protocol (which defaults to
    soap12), and \a method (which defaults to POST). Requires specifying
    other params later (setParameters()).

    \sa setParameters(), setProtocol(), invokeMethod()
  */
QWebMethod::QWebMethod(QObject *parent, Protocol protocol, HttpMethod method) :
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

    \sa setParameters(), setProtocol(), invokeMethod()
  */
QWebMethod::QWebMethod(const QUrl &url, Protocol protocol,
                       HttpMethod method, QObject *parent) :
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
QWebMethod::QWebMethod(QWebMethodPrivate &dd, Protocol protocol,
                       HttpMethod httpMethod, QObject *parent) :
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
    \fn QWebMethod::replyReady(const QByteArray &rply)

    Signal invoked when the reply (\a rply) from web service's server
    is ready for reading.
  */

/*!
    \fn QWebMethod::hostChanged()

    Signal invoked when host value is changed.
  */

/*!
    \fn QWebMethod::hostUrlChanged()

    Signal invoked when hostUrl value is changed.
  */

/*!
    \fn QWebMethod::nameChanged()

    Signal invoked when method name is changed.
  */

/*!
    \fn QWebMethod::targetNamespaceChanged()

    Signal invoked when targetNamespace value is changed.
  */

/*!
    \fn QWebMethod::parameterNamesChanged()

    Signal invoked when parameters are changed.
  */

/*!
    \fn QWebMethod::protocolChanged()

    Signal invoked when protocol value is changed.
  */

/*!
    \fn QWebMethod::httpMethodChanged()

    Signal invoked when httpMethod value is changed.
  */


/*!
    Returns host's URL (in QString). If you want a QUrl, call getHostUrl(),
    or QUrl(QWebMethod::getHost());

    \sa hostUrl(), setHost()
  */
QString QWebMethod::host() const
{
    Q_D(const QWebMethod);
    return d->m_hostUrl.path();
}

/*!
    Returns host's URL. If you want a QString, call getHost() or getHostUrl().host();

    \sa host(), setHost()
  */
QUrl QWebMethod::hostUrl() const
{
    Q_D(const QWebMethod);
    return d->m_hostUrl;
}

/*!
    Sets message's host to \a newHost.

    \sa host(), hostUrl()
  */
void QWebMethod::setHost(const QString &newHost)
{
    Q_D(QWebMethod);
    d->m_hostUrl.setPath(newHost);
    emit hostChanged();
}

/*!
    \overload
    Sets message's host to \a newHost.

    \sa host(), hostUrl()
  */
void QWebMethod::setHost(const QUrl &newHost)
{
    Q_D(QWebMethod);
    d->m_hostUrl = newHost;
    emit hostUrlChanged();
}

/*!
    Returns username, which is used for authentication.

    \sa authenticate(), setUsername(), setCredentials()
  */
QString QWebMethod::username() const
{
    Q_D(const QWebMethod);
    return d->m_username;
}

/*!
    Used for authentication. Sets username using \a newUsername.
    To authenticate, call authenticate().

    \sa setPassword(), setCredentials(), authenticate(), username()
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

    \sa setUsername(), setPassword(), authenticate(), username()
  */
void QWebMethod::setCredentials(const QString &newUsername, const QString &newPassword)
{
    Q_D(QWebMethod);
    d->m_username = newUsername;
    d->m_password = newPassword;
}

/*!
    Performs authentication using \a newUsername and \a newPassword,
    if specified. If not, and they were given using setCredentials(),
    setUsername() or setPassword(), it uses the existing values.

    If no data is specified, it does nothing. Returns true on success.

    \sa setCredentials(), setUsername(), setPassword(), username()
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
        url.addEncodedQueryItem("ACT", QUrl::toPercentEncoding(QLatin1String("11")));
        url.addEncodedQueryItem("RET", QUrl::toPercentEncoding(QLatin1String("/")));
        url.addEncodedQueryItem("site_id", QUrl::toPercentEncoding(QLatin1String("1")));
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

    If empty data is specified, it does nothing (and returns false).
    Returns true on success.

    \sa setCredentials(), setUsername(), setPassword(), username()
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
                             QString(QLatin1String("http://")
                                     + d->m_hostUrl.host()
                                     + QLatin1String("/"))));
    rqst.setHeader(QNetworkRequest::ContentTypeHeader,
                   QLatin1String("application/x-www-form-urlencoded"));

    QByteArray paramBytes = customAuthString.toString().mid(1).toLatin1();
    paramBytes.replace("/", "%2F");

    d->manager->post(rqst, paramBytes);
    return true;
}

/*!
    Returns method's name.
  */
QString QWebMethod::methodName() const
{
    Q_D(const QWebMethod);
    return d->m_methodName;
}

/*!
    Sets method's name to \a newName.
  */
void QWebMethod::setMethodName(const QString &newName)
{
    Q_D(QWebMethod);
    d->m_methodName = newName;
    emit nameChanged();
}

/*!
    Returns list of parameters' names.

    \sa parameterNamesTypes(), setParameters()
  */
QStringList QWebMethod::parameterNames() const
{
    Q_D(const QWebMethod);
    return (QStringList) d->parameters.keys();
}

/*!
    Returns whole parameter information (name and type).

    \sa parameterNames(), setParameters()
  */
QMap<QString, QVariant> QWebMethod::parameterNamesTypes() const
{
    Q_D(const QWebMethod);
    return d->parameters;
}

/*!
    Sets method's parameters (\a params).
    This also includes their names (as map key).

    \sa parameterNamesTypes(), parameterNames()
  */
void QWebMethod::setParameters(const QMap<QString, QVariant> &params)
{
    Q_D(QWebMethod);
    d->parameters = params;
    emit parameterNamesChanged();
}

/*!
    Returns return value's name.

    \sa returnValueNameType(), setReturnValue()
  */
QStringList QWebMethod::returnValueName() const
{
    Q_D(const QWebMethod);
    return (QStringList) d->returnValue.keys();
}

/*!
    Returns whole return value information (name and type).

    \sa returnValueName(), setReturnValue()
  */
QMap<QString, QVariant> QWebMethod::returnValueNameType() const
{
    Q_D(const QWebMethod);
    return d->returnValue;
}

/*!
    Sets method's return value (\a returnVal). This also includes their names.

    \sa returnValueNameType(), returnValueName()
  */
void QWebMethod::setReturnValue(const QMap<QString, QVariant> &returnVal)
{
    Q_D(QWebMethod);
    d->returnValue = returnVal;
}

/*!
    Returns target namespace.

    Target namespace is used in SOAP messaging.

    \sa setTargetNamespace()
  */
QString QWebMethod::targetNamespace() const
{
    Q_D(const QWebMethod);
    return d->m_targetNamespace;
}

/*!
    Sets method's target namespace (\a tNamespace),
    which is needed in SOAP messaging.

    \sa targetNamespace()
  */
void QWebMethod::setTargetNamespace(const QString &tNamespace)
{
    Q_D(QWebMethod);
    d->m_targetNamespace = tNamespace;
    emit targetNamespaceChanged();
}

/*!
    Returns currently set protocol.

    \sa protocolString(), httpMethod()
  */
QWebMethod::Protocol QWebMethod::protocol() const
{
    Q_D(const QWebMethod);
    return d->protocolUsed;
}

/*!
    Returns protocol used in form of a QString. If \a includeRest is
    true, and --rest flag was specified, it appends ",rest" to the result.

    \sa protocol()
  */
QString QWebMethod::protocolString(bool includeRest) const
{
    Q_D(const QWebMethod);
    QString result;

    if (d->protocolUsed & Http)
        result = QLatin1String("Http");
    else if (d->protocolUsed & Soap10)
        result = QLatin1String("Soap10");
    else if (d->protocolUsed & Soap12)
        result = QLatin1String("Soap12");
    else if (d->protocolUsed & Json)
        result = QLatin1String("Json");
    else if (d->protocolUsed & Xml)
        result = QLatin1String("Xml");

    if (includeRest && (d->protocolUsed & Rest))
        result += QLatin1String(",rest");

    return result;
}

/*!
    Sets the protocol flag (\a prot, being one of the values
    of QWebMethod::Protocol). This determines the protocol used later,
    when sending request. Defaults to SOAP1.2.

    Returns true if allowed combination was specified.

    WARNING:
    This method also checks for disallowed combinations. If such combination
    is encountered, it discards the set protocol, and sets SOAP 1.2.
    An example of dissalowed combination is setting JSON and SOAP
    simultaneously.
  */
bool QWebMethod::setProtocol(Protocol prot)
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

        emit protocolChanged();
        return true;
    } else {
//        d->enterErrorState(QLatin1String("Wrong protocol is set. You have "
//                                            "combined exclusive flags."));
        d->protocolUsed = Soap12;
        emit protocolChanged();
        return false;
    }
}

/*!
    \overload

    Sets the protocol flag by parsing the given string, and passing it to setProtocol()
    as a QWebMethod flag. This determines the protocol used later,
    when sending request. Defaults to SOAP1.2.

    Returns true if allowed combination was specified.

    WARNING:
    This method also checks for disallowed combinations. If such combination
    is encountered, it discards the set protocol, and sets SOAP 1.2.
    An example of dissalowed combination is setting JSON and SOAP
    simultaneously.
  */
bool QWebMethod::setProtocol(QString protocolString)
{
    QString protocol = protocolString.toLower();
    QStringList protocolList;
    Protocol result = Soap;// = 0;

    // For 2 protocol setup.
    if (protocol.contains(QLatin1Char(',')))
        protocolList = protocol.split(QLatin1Char(','));
    else
        protocolList.append(protocol);

    if (protocolList.contains(QLatin1String("http"), Qt::CaseInsensitive))
        result = Http;
    else if (protocolList.contains(QLatin1String("soap10"), Qt::CaseInsensitive))
        result = Soap10;
    else if (protocolList.contains(QLatin1String("soap12"), Qt::CaseInsensitive))
        result = Soap12;
    else if (protocolList.contains(QLatin1String("soap"), Qt::CaseInsensitive))
        result = Soap;
    else if (protocolList.contains(QLatin1String("xml"), Qt::CaseInsensitive))
        result = Xml;
    else if (protocolList.contains(QLatin1String("json"), Qt::CaseInsensitive))
        result = Json;

    if (protocolList.contains(QLatin1String("rest"), Qt::CaseInsensitive))
        return (Rest | result);

    return setProtocol(result);
}

/*!
    Returns currently set HTTP method.

    \sa httpMethodString(), protocol()
  */
QWebMethod::HttpMethod QWebMethod::httpMethod() const
{
    Q_D(const QWebMethod);
    return d->httpMethodUsed;
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
        result = QLatin1String("Post");
    else if (d->httpMethodUsed == Get)
        result = QLatin1String("Get");
    else if (d->httpMethodUsed == Put)
        result = QLatin1String("Put");
    else if (d->httpMethodUsed == Delete)
        result = QLatin1String("Delete");

    return result;
}

/*!
    Sets the httpMethod flag (\a method, a value of QWebMethod::HttpMethod).
    Default method is QWebMethod::Post.
  */
void QWebMethod::setHttpMethod(HttpMethod method)
{
    Q_D(QWebMethod);
    d->httpMethodUsed = method;
    emit httpMethodChanged();
}

/*!
    \overload

    Sets the httpMethod flag (\a newMethod, using QString representation
    of HTTP method (post, get, put, or delete)). Setting is NOT case sensitive,
    so you can pass "pOSt", and it will be recognised as POST method.
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

    emit httpMethodChanged();
    return true;
}

/*!
    Invokes the method asynchronously, assuming that all neccessary data was
    specified earlier. Optionally, a QByteArray (\a requestData) can be
    specified - it will override standard data encapsulation (preparation,
    see prepareRequestData()), and send the byte array without any changes.

    If synchronous operation is needed, you can:
    \list
        \o use static QWebServiceMethod::invokeMethod()
        \o subclass QWebMethod, and add a static invokeMethod() method with
           a "waiting loop"
        \o wait for replyReady() signal in a "waiting loop"
    \endlist

    Here's a waiting loop snippet:
    \code
    QWebMethod qsm;
    ...
    qsm.invokeMethod();
    forever {
        if (qsm.isReplyReady()) {
            return qsm.replyRead();
        } else {
            // Ensures that application remains responive to events (prevents freezing).
            qApp->processEvents();
        }
    }
    \endcode

    Returns true on success.

    \sa setParameters(), setProtocol(), setTargetNamespace()
  */
bool QWebMethod::invokeMethod(const QByteArray &requestData)
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
                QCoreApplication::instance()->processEvents();
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
    After making asynchronous call, and getting the replyReady() signal,
    this method can be used to read the reply.

    This method returns QString with complete and exact reply from server.
    If other data format is needed, other methods can be used:
    \list
    \o for parsed reply, stored in QVariant - replyReadParsed()
    \o for raw, unprocessed reply - replyReadRaw()
    \endlist

    \sa replyReadParsed(), replyReadRaw()
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

    Returns parsed data (with type specified in WSDL or by user, wrapped
    in QVariant). Tries to parse the reply automatically, but specifying
    reply name and type using setReturnValue() might help a lot (this is
    relevant mostly for SOAP).

    Web APIs usually return a lot of data
    in one go, this method is likely to get lost in it. It expects a simple
    reply (say, you request 2 + 2, and get SOAP-enveloped reply - it will
    work fine then, but not necessarily if WS returns a lot of additional
    information).

    \sa replyRead(), replyReadRaw()
  */
QVariant QWebMethod::replyReadParsed()
{
    Q_D(QWebMethod);
    QVariant result;
    QByteArray replyBytes = d->reply;
    QString replyString = d->convertReplyToUtf(replyBytes);

    // It's not done properly, anyway.
    // Should return type specified in replyValue.
    if (d->protocolUsed & Soap || d->protocolUsed & Xml) {
        QString tempBegin = QString(QLatin1String("<")
                                    + d->m_methodName);
//                                    + QLatin1String("Result>"));
        int replyBeginIndex = replyString.indexOf(tempBegin, 0,
                                                  Qt::CaseSensitive);
        replyBeginIndex += tempBegin.length();

        QString tempFinish = QString(QLatin1String("</")
                                     + d->m_methodName);
//                                     + QLatin1String("Result>"));
        int replyFinishIndex = replyString.indexOf(tempFinish,
                                                   replyBeginIndex,
                                                   Qt::CaseSensitive);

        if (replyBeginIndex == -1)
            replyBytes = 0;
        if (replyFinishIndex == -1)
            replyFinishIndex = replyString.length();

        if (d->returnValue.isEmpty()) {
            result = (QVariant) replyString.mid(replyBeginIndex,
                                                replyFinishIndex - replyBeginIndex);
        } else {
            // This attempts to prepare a complete list of replies, if enough data is
            // specified in returnValue QMap.
            QString replyCore = replyString.mid(replyBeginIndex,
                                                replyFinishIndex - replyBeginIndex);

            // Need to add replyCore splitting into specific return values.
            QList<QVariant> parsedReturns;
            QStringList returnsSplitted;
            // This is an optimistic algorithm, it assumes that
            // returnValue QMap is right.
            foreach (QString s, d->returnValue.keys()) {
                if (replyCore.contains(s)) {
                    // Get tag beginning index.
                    int tempIndex = replyCore.indexOf(s, 0, Qt::CaseSensitive);
                    // Get tag ending index.
                    int tempBeginIndex = replyCore.indexOf(">", tempIndex, Qt::CaseSensitive) + 1;
                    // Get closing tag index.
                    tempIndex = replyCore.indexOf("</", tempBeginIndex, Qt::CaseSensitive);

                    QString value = replyCore.mid(tempBeginIndex, tempBeginIndex - tempIndex).trimmed();
                    returnsSplitted.append(value);
                }
            }

            int i = 0;
            foreach (QString s, d->returnValue.keys()) {
                QString type = d->returnValue.value(s).typeName();

                if (type == QLatin1String("int")) {
                    parsedReturns.append(QVariant(returnsSplitted.at(i).toInt()));
                } else if (type == QLatin1String("float")) {
                    parsedReturns.append(QVariant(returnsSplitted.at(i).toFloat()));
                } else if (type == QLatin1String("double")) {
                    parsedReturns.append(QVariant(returnsSplitted.at(i).toDouble()));
                } else if (type == QLatin1String("bool")) {
                    if (returnsSplitted.at(i).toLower() == QLatin1String("true"))
                        parsedReturns.append(QVariant(true));
                    else if (returnsSplitted.at(i).toLower() == QLatin1String("false"))
                        parsedReturns.append(QVariant(true));
                } else if (type == QLatin1String("QDateTime")) {
                    ;
                } else if ((type == QLatin1String("QString"))
                           || (type == QLatin1String("QChar"))) {
                    parsedReturns.append(QVariant(returnsSplitted.at(i)));
                } else if (type == QLatin1String("QStringList")) {
                    // Prepare QStringLists
                } else {
                    parsedReturns.append(returnsSplitted.at(i));
                }

                i++;
            }

            if (parsedReturns.size() > 1)
                result = parsedReturns;
            else
                result = parsedReturns.first();
        }
    } else if (d->protocolUsed & Json) {
        // Parse JSON if you dare. Qt5 will have JSON parser, I could implement that then.. maybe.
        // Writing own parser right now seems pointless.
    } else { // Fallback - return QString. Will also be used for HTTP, which is bad.
        result = replyString;
    }

    return replyBytes;
}

/*!
    After making asynchronous call, and getting the replyReady() signal,
    this method can be used to read the reply.

    Returns the raw data acquired from server.

    \sa replyRead(), replyReadParsed()
  */
QByteArray QWebMethod::replyReadRaw()
{
    Q_D(QWebMethod);
    return d->reply;
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
    Returns true if reply was received,
    and is ready to be read.
  */
bool QWebMethod::isReplyReady() const
{
    Q_D(const QWebMethod);
    return d->replyReceived;
}

/*!
    Protected slot, which processes
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
        d->enterErrorState(QString(QLatin1String("Authentication error! ")
                                   + reply->readAll()));
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
    Private function, invoked by invokeMethod(). Modifies QByteArray data,
    so that it is consistent with protocol and HTTP method specification.
    It uses QMap<QString, QVariant> parameters to fill data object's body.
    Can be overriden by creating custom QByteArray and passing it to
    sendMessage().

    \sa invokeMethod()
  */
void QWebMethodPrivate::prepareRequestData()
{
    data.clear();
    QString header, body, footer;
    // Replace with something OS-independent, or seriously rethink.
    QString endl = QLatin1String("\r\n");

    if (protocolUsed & QWebMethod::Soap) {
        if (protocolUsed & QWebMethod::Soap12) {
            header = QString(QLatin1String("<?xml version=\"1.0\" encoding=\"utf-8\"?> ")
                     + endl + QLatin1String(" <soap12:Envelope "
                     "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
                     "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" "
                     "xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\"> ") + endl +
                     QLatin1String(" <soap12:Body> ") + endl);

            footer = QString(QLatin1String("</soap12:Body> ")
                             + endl
                             + QLatin1String("</soap12:Envelope>"));
        } else if (protocolUsed & QWebMethod::Soap10) {
            header = QString(QLatin1String("<?xml version=\"1.0\" encoding=\"utf-8\"?> ")
                     + endl + QLatin1String(" <soap:Envelope "
                     "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
                     "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" "
                     "xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\"> ") + endl +
                     QLatin1String(" <soap:Body> ") + endl);

            footer = QString(QLatin1String("</soap:Body> ")
                             + endl
                             + QLatin1String("</soap:Envelope>"));
        }

        body = QString(QLatin1String("\t<") + m_methodName
                       + QLatin1String(" xmlns=\"")
                       + m_targetNamespace
                       + QLatin1String("\"> ") + endl);

        foreach (const QString currentKey, parameters.keys()) {
            QVariant qv = parameters.value(currentKey);
            // Currently, this does not handle nested lists
            body += QString(QLatin1String("\t\t<") + currentKey
                            + QLatin1String(">")
                            + qv.toString()
                            + QLatin1String("</") + currentKey
                            + QLatin1String("> ") + endl);
        }

        body += QString(QLatin1String("\t</")
                        + m_methodName
                        + QLatin1String("> ") + endl);
    } else if (protocolUsed & QWebMethod::Http) {
        foreach (const QString currentKey, parameters.keys()) {
            QVariant qv = parameters.value(currentKey);
            // Currently, this does not handle nested lists
            body += QString(currentKey + QLatin1String("=")
                            + qv.toString() + QLatin1String("&"));
        }
        body.chop(1);
    } else if (protocolUsed & QWebMethod::Json) {
        body += QString(QLatin1String("{") + endl);
        foreach (const QString currentKey, parameters.keys()) {
            QVariant qv = parameters.value(currentKey);
            // Currently, this does not handle nested lists
            body += QString(QLatin1String("{") + endl
                            + QLatin1String("\t\"") + currentKey
                            + QLatin1String("\" : \"")
                            + qv.toString()
                            + QLatin1String("\"") + endl);
        }
        body += QLatin1String("}");
    } else if (protocolUsed & QWebMethod::Xml) {
        foreach (const QString currentKey, parameters.keys()) {
            QVariant qv = parameters.value(currentKey);
            // Currently, this does not handle nested lists
            body += QString(QLatin1String("\t\t<") + currentKey
                            + QLatin1String(">") + qv.toString()
                            + QLatin1String("</") + currentKey
                            + QLatin1String("> ") + endl);
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
    errorMessage += QString(errMessage + QLatin1String(" "));
    emit q->errorEncountered(errMessage);
    return false;
}
