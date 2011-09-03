#include "../headers/qwebmethod.h"

/*!
    \class QWebMethod
    \brief Class that can be used to asnchronously and synchronously send HTTP, SOAP 1.0, SOAP 1.2, JSON, XMLand REST messages to web services.

    QWebMethod class is acting as web method. You can use it to send and retrieve data from web services. There are
    many helper classes. QWsdl can be used to read a WSDL file - it will automatically create a QMap of QWebMethods,
    all already set up and ready to be invoked. QWebService can be used similarily as it is a high-level wrapper
    to QWsdl. Additionally, qtWsdlConverter console application can automatically generate custom classes
    for any WSDL file.

    Typically, to send a message, you will need to set the URL, message name, target namespace (when using SOAP),
    parameters list (when invoking a medhod that has parameters), and then use the parameterless sendMessage()
    method. Here is a snippet for that:
    \code
    QWebMethod *message = new QWebMethod(0, QWebMethod::soap12, QWebMethod::POST);
    message->setHost("http://www.currencyserver.de/webservice/currencyserverwebservice.asmx");
    message->setMessageName("getProviderList");
    message->setTargetNamespace("http://www.daenet.de/webservices/CurrencyServer");
    message->sendMessage();
    \endcode
    You then have to wait for replyReady(QVariant) signal, or check for reply using isReplyReady() convenience
    method.

    To send a REST message with (for example) JSON body, pass (QWebMethod::rest | QWebMethod::json) as protocol flag.
    Additionally, specify HTTP method to be used (POST, GET, PUT, DELETE). When sending a REST message, \a messageName
    is used as request URI, and \a parameters specify additioanl data to be sent in message body.

    This class provides asynchronous sendMessage() only. If synchronous operation is needed, you can:
    \list
        \o use QWebServiceMethod, which provides some additional, useful constructors and methods
        \o use static QWebServiceMethod::sendMessage()
        \o subclass QWebMethod, and add a static sendMessage() method with a "waiting loop"
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
            qApp->processEvents(); // Ensures that application remains responive to events (prevents freezing).
        }
    }
    \endcode

    If you want to save some time on configuration in your code, you can subclass QWebMethod, implement a method, that
    would store your settings (an example configure() method is described below). Then, create your own slot
    for parsing the reply.

    In your "settings method", named for example configure(), you may want to:
    \list
        \o set host Url
        \o set protocol
        \o set message name
        \o set target namespace
        \o connect replyReady() signal to your custom slot (where you can parse the reply
           and return it in a type of your convenience)
    \endlist

    \sa init()
  */

/*!
     \enum QWebMethod::Protocol

     This enum type specifies the protocol that QWebMethod will use in communicating with the server:

     \value http
            HTTP protocol will be used.
     \value soap10
            SOAP 1.0 will be used.
     \value soap12
            SOAP 1.2 will be used.
     \value soap
            Convenience SOAP aggregator, used internally. If passed to setProtocol(), SOAP 1.2 will be used instead.
     \value json
            JSON message will be used.
     \value xml
            Message will be sent using plain XML (much simpler than SOAP).
     \value rest
            QWebMethod will switch into REST mode. This can be combined with any other Protocol flag to define, what message body should look like.
 */

/*!
  \enum QWebMethod::HttpMethod

  Defines HTTP method to use when sending the message. Using more than 1 at the same time is forbidden
  (will not work).

  \value POST
         QWebMethod will use POST.
  \value GET
         QWebMethod will use GET.
  \value PUT
         QWebMethod will use PUT.
  \value DELETE
         QWebMethod will use DELETE.
  */

/*!
    \fn QWebMethod::QWebMethod(QObject *parent, Protocol protocol, HttpMethod method)

    Constructs the message usign \a parent, \a protocol (which defaults to soap12),
    and \a method (which defaults to POST). Requires specifying other params later (setParameters()).

    \sa init(), setParameters(), setProtocol(), sendMessage()
  */
QWebMethod::QWebMethod(QObject *parent, Protocol protocol, HttpMethod method) :
    QObject(parent)
{
    init();
    setProtocol(protocol);
    setHttpMethod(method);
    m_hostUrl.setHost("");
    m_messageName = "";
    parameters.clear();
}

/*!
    \fn QWebMethod::~QWebMethod()

    Deletes internal pointers.
  */
QWebMethod::~QWebMethod()
{
    delete manager;
//    this->deleteLater();
}

/*!
    \fn QWebMethod::errorEncountered(QString errMessage)

    Singal emitted when WsdlConverter encounters an error. Carries \a errMessage for convenience.
  */

/*!
  \fn QWebMethod::setHost(QString newHost)

  Sets message's host to \a newHost.
  */
void QWebMethod::setHost(QString newHost)
{
    m_hostUrl.setPath(newHost);
}

/*!
  \fn QWebMethod::setHost(QUrl newHost)

  Sets message's host to \a newHost.
  */
void QWebMethod::setHost(QUrl newHost)
{
    m_hostUrl = newHost;
}

/*!
  \fn QWebMethod::setUsername(QString newUsername)

  Used for authentication. Sets username using \a newUsername.

  \sa setPassword(), setCredentials()
  */
void QWebMethod::setUsername(QString newUsername)
{
    m_username = newUsername;
}

/*!
  \fn QWebMethod::setPassword(QString newPassword)

  Used for authentication. Sets password using \a newPassword.

  \sa setUsername(), setCredentials()
  */
void QWebMethod::setPassword(QString newPassword)
{
    m_password = newPassword;
}

/*!
  \fn QWebMethod::setCredentials(QString newUsername, QString newPassword)

  Used for authentication. Sets username (\a newUsername) and password (\a newPassword).

  \sa setUsername(), setPassword()
  */
void QWebMethod::setCredentials(QString newUsername, QString newPassword)
{
    m_username = newUsername;
    m_password = newPassword;
}

/*!
  \fn QWebMethod::setMessageName(QString newName)

  Sets message's name to \a newName.
  */
void QWebMethod::setMessageName(QString newName)
{
    m_messageName = newName;
}

/*!
  \fn QWebMethod::setMethodName(QString newName)

  Sets message's name to \a newName.
  */
void QWebMethod::setMethodName(QString newName)
{
    m_messageName = newName;
}

/*!
    \fn QWebMethod::setParameters(QMap<QString, QVariant> params)

    Sets method's parameters (\a params). This also includes their names.
  */
void QWebMethod::setParameters(QMap<QString, QVariant> params)
{
    parameters = params;
}

/*!
    \fn QWebMethod::setReturnValue(QMap<QString, QVariant> returnVal)

    Sets method's return value (\a returnVal). This also includes their names.
  */
void QWebMethod::setReturnValue(QMap<QString, QVariant> returnVal)
{
    returnValue = returnVal;
}

/*!
    \fn QWebMethod::setTargetNamespace(QString tNamespace)

    Sets message's target namespace (\a tNamespace), which is needed in SOAP messaging.
  */
void QWebMethod::setTargetNamespace(QString tNamespace)
{
    m_targetNamespace = tNamespace;
}

/*!
    \fn QWebMethod::setProtocol(Protocol prot)

    Sets the protocol flag (\a prot, being one of the values of QWebMethod::Protocol).
    This determines the protocol used later, when sending request. Defaults to SOAP1.2.

    WARNING:
    This method also checks for disallowed combinations. If such combination is encountered,
    it discards the set protocol, and sets SOAP 1.2.

    This behavious will probably change in the future, when QWebMethod gets error handling.
  */
void QWebMethod::setProtocol(Protocol prot)
{
    // Prevent incompatibile flags from being set simultaneously:
    QList<int> allowedCombinations;
    allowedCombinations << 0x01 << 0x02 << 0x04 << 0x06 << 0x08 << 0x10 << 0x20; // Standard values.
    allowedCombinations << 0x21 << 0x22 << 0x24 << 0x26 << 0x28 << 0x30; // REST combinations

    if (allowedCombinations.contains(prot)) {
        if (prot & soap)
            protocolUsed = soap12;
        else
            protocolUsed = prot;
    }
    else {
        enterErrorState("Wrong protocol is set. You have combined exclusive flags.");
//        protocolUsed = soap12;
    }
}

/*!
  \fn QWebMethod::setHttpMethod(HttpMethod method)

  Sets the httpMethod flag (\a method, a value of QWebMethod::HttpMethod).
  Default method is QWebMethod::POST.
  */
void QWebMethod::setHttpMethod(HttpMethod method)
{
    httpMethodUsed = method;
}

/*!
  \fn QWebMethod::setHttpMethod(QString newMethod)

  Sets the httpMethod flag (\a newMethod, using QString representation of HTTP method
  (post, get, put, or delete)). Setting is NOT case sensitive.
  Default method is POST.

  Returns true if successful.
  */
bool QWebMethod::setHttpMethod(QString newMethod)
{
    if (newMethod.toLower() == "post")
        httpMethodUsed = POST;
    else if (newMethod.toLower() == "get")
        httpMethodUsed = GET;
    else if (newMethod.toLower() == "put")
        httpMethodUsed = PUT;
    else if (newMethod.toLower() == "delete")
        httpMethodUsed = DELETE;
    else
        return false;

    return true;
}

/*!
    \fn bool QWebMethod::sendMessage(QByteArray requestData)

    Sends the message asynchronously, assuming that all neccessary data was specified earlier.
    Optionally, a QByteArray (\a requestData) can be specified - it will override standard
    data encapsulation (preparation, see prepareRequestData()), and send the byte array without any changes.

    If synchronous operation is needed, you can:
    \list
        \o use static QWebServiceMethod::sendMessage()
        \o subclass QWebMethod, and add a static sendMessage() method with a "waiting loop"
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
            qApp->processEvents(); // Ensures that application remains responive to events (prevents freezing).
        }
    }
    \endcode

    Returns true on success.

    \sa setParameters(), setProtocol(), setTargetNamespace(), prepareRequestData()
  */
bool QWebMethod::sendMessage(QByteArray requestData)
{
    // Auth workaround.
    if (m_username != "") {
        authReply = false;
        disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(authReplyFinished(QNetworkReply*)));

        QNetworkRequest rqst(QUrl::fromUserInput("http://developer.qt.nokia.com/"));
        qDebug() << rqst.url().toString();
        rqst.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

        QUrl url;
        url.addEncodedQueryItem("ACT", QUrl::toPercentEncoding("11"));
        url.addEncodedQueryItem("RET", QUrl::toPercentEncoding("/"));
        url.addEncodedQueryItem("site_id", QUrl::toPercentEncoding("1"));
        url.addEncodedQueryItem("username", QUrl::toPercentEncoding(m_username));
        url.addEncodedQueryItem("password", QUrl::toPercentEncoding(m_password));

        QByteArray paramBytes = url.toString().mid(1).toLatin1();
        paramBytes.replace("/", "%2F");
        qDebug() << url.toString().mid(1).toLatin1();;

        manager->post(rqst, paramBytes);

        forever {
            if (authReply) {
                qDebug() << "Continuing";
                break;
            }
            else {
                qApp->processEvents();
            }
        }

        disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(authReplyFinished(QNetworkReply*)));
        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    }
    // EOAuth workaround.

    QNetworkRequest request;
    request.setUrl(m_hostUrl);

    if (protocolUsed & soap)
        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/soap+xml; charset=utf-8"));
//    else if (protocolUsed & json)
//        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json; charset=utf-8"));
    else if (protocolUsed & http)
        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("Content-Type: application/x-www-form-urlencoded"));
    else if (protocolUsed & xml)
        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/xml; charset=utf-8"));

    if (protocolUsed & soap10)
        request.setRawHeader(QByteArray("SOAPAction"), QByteArray(m_hostUrl.toString().toAscii()));

    if (requestData.isNull() || requestData.isEmpty())
        prepareRequestData();
    else
        data = requestData;

    // OPTIONAL - FOR TESTING:
//    qDebug() << request.url().toString();
//    qDebug() << QString(data);
    // ENDOF: OPTIONAL - FOR TESTING

    if (protocolUsed & rest) {
        if (httpMethodUsed == POST)
            manager->post(request, data);
        else if (httpMethodUsed == GET)
            manager->get(request);
        else if (httpMethodUsed == PUT)
            manager->put(request, data);
        else if (httpMethodUsed == DELETE)
            manager->deleteResource(request);
    }
    else {
        manager->post(request, data);
    }

    return true;
}

void QWebMethod::authReplyFinished(QNetworkReply *reply)
{
    authReply = true;
    QByteArray array = reply->readAll();
        if (array.isEmpty())
        {
            qDebug() << "Login correct";
        }
}

/*!
    \fn QWebMethod::replyRead()

    After making asynchronous call, and getting the replyReady() signal, this method can be used to read the reply.
  */
QVariant QWebMethod::replyRead()
{
    return reply;
}

/*!
    \fn QWebMethod::replyReady(QVariant rply)

    Signal invoked when the reply (\a rply) from web service's server is ready for reading.
  */

/*!
    \fn QWebMethod::messageName() const

    Returns message's name.
  */
QString QWebMethod::messageName() const
{
    return m_messageName;
}

/*!
    \fn QWebMethod::parameterNames() const

    Retrurns list of parameters' names.
  */
QStringList QWebMethod::parameterNames() const
{
    return (QStringList) parameters.keys();
}

/*!
    \fn QWebMethod::returnValueName() const

    Returns return value's name.
  */
QStringList QWebMethod::returnValueName() const
{
    return (QStringList) returnValue.keys();
}

/*!
    \fn QWebMethod::parameterNamesTypes() const

    Returns whole parameter information (name and type).
  */
QMap<QString, QVariant> QWebMethod::parameterNamesTypes() const
{
    return parameters;
}

/*!
    \fn QWebMethod::returnValueNameType() const

    Returns whole return value information (name and type).
  */
QMap<QString, QVariant> QWebMethod::returnValueNameType() const
{
    return returnValue;
}

/*!
    \fn QWebMethod::targetNamespace() const

    Returns target namespace.
  */
QString QWebMethod::targetNamespace() const
{
    return m_targetNamespace;
}

/*!
    \fn QWebMethod::host() const

    Returns host's URL (in QString). If you want a QUrl, call getHostUrl(), or QUrl(QWebMethod::getHost());
  */
QString QWebMethod::host() const
{
    return m_hostUrl.path();
}

/*!
    \fn QWebMethod::hostUrl() const

    Returns host's URL. If you want a QString, call getHost() or getHostUrl().host();
  */
QUrl QWebMethod::hostUrl() const
{
    return m_hostUrl;
}

/*!
  \fn QWebMethod::username() const

  Returns username.
  */
QString QWebMethod::username() const
{
    return m_username;
}

/*!
  \fn QWebMethod::protocol() const

  Returns currently set protocol.

  \sa httpMethod()
  */
QWebMethod::Protocol QWebMethod::protocol() const
{
    return protocolUsed;
}

/*!
  \fn QWebMethod::httpMethod() const

  Returns currently set HTTP method.

  \sa protocol()
  */
QWebMethod::HttpMethod QWebMethod::httpMethod() const
{
    return httpMethodUsed;
}

/*!
  \fn QWebMethod::protocolString(bool includeRest) const

  Returns protocol used in form of a QString. If \a includeRest is
  true, and --rest flag was specified, it appends ",rest" to the result.
  */
QString QWebMethod::protocolString(bool includeRest) const
{
    QString result = "";

    if (protocolUsed & http)
        result = "http";
    else if (protocolUsed & soap10)
        result = "soap10";
    else if (protocolUsed & soap12)
        result = "soap12";
    else if (protocolUsed & json)
        result = "json";
    else if (protocolUsed & xml)
        result = "xml";

    if (includeRest && (protocolUsed & rest))
        result += ",rest";

    return result;
}

/*!
  \fn QWebMethod::httpMethodString() const

  Returns the HTTP method used, in form of a QString.

  \sa httpMethod()
  */
QString QWebMethod::httpMethodString() const
{
    QString result = "";

    if (httpMethodUsed == POST)
        result = "POST";
    else if (httpMethodUsed == GET)
        result = "GET";
    else if (httpMethodUsed == PUT)
        result = "PUT";
    else if (httpMethodUsed == DELETE)
        result = "DELETE";

    return result;
}

/*!
    \fn QWebMethod::errorInfo() const

    Returns QString with error message in case an error occured. Otherwise, returns empty string.

    \sa isErrorState()
  */
QString QWebMethod::errorInfo() const
{
    return errorMessage;
}

/*!
    \fn QWebMethod::isErrorState() const

    Returns true if there was an error, false otherwise. Details about an error can be read with
    getErrorInfo().

    \sa errorInfo()
  */
bool QWebMethod::isErrorState() const
{
    return errorState;
}

/*!
  \fn QWebMethod::isReplyReady() const

  Returns true if reply was received.
  */
bool QWebMethod::isReplyReady() const
{
    return replyReceived;
}

/*!
    \fn QWebMethod::replyFinished(QNetworkReply *netReply)

    Public (will probably be private in the future) slot, which processes the reply (\a netReply) from the server.
    Emits the replyReady() signal.
  */
void QWebMethod::replyFinished(QNetworkReply *netReply)
{
    QByteArray replyBytes;

    replyBytes = (netReply->readAll());
    QString replyString = convertReplyToUtf(replyBytes);

    // This section is SOAP-only and should be fixed for other protocols!
//    QString tempBegin = "<" + m_messageName + "Result>";
//    int replyBeginIndex = replyString.indexOf(tempBegin, 0, Qt::CaseSensitive);
//    replyBeginIndex += tempBegin.length();

//    QString tempFinish = "</" + m_messageName + "Result>";
//    int replyFinishIndex = replyString.indexOf(tempFinish, replyBeginIndex, Qt::CaseSensitive);

//    if (replyBeginIndex == -1)
//        replyBytes = 0;
//    if (replyFinishIndex == -1)
//        replyFinishIndex = replyString.length();
    // EO section.

//    reply = (QVariant) replyString.mid(replyBeginIndex, replyFinishIndex - replyBeginIndex);
    reply = replyString;

    replyReceived = true;
    emit replyReady(reply);
}

/*!
  \fn QWebMethod::authenticate(QNetworkReply *reply, QAuthenticator *authenticator)

  Internal method used to authenticate the communication. Use setCredentials() or setUsername()
  and setPassword() to specify the data.
  */
void QWebMethod::authenticate(QNetworkReply *reply, QAuthenticator *authenticator)
{
    if (authenticationError)
    {
        enterErrorState("Authentication error! " + reply->readAll());
        return;
    }

    authenticator->setUser(m_username);
    authenticator->setPassword(m_password);
    authenticationError = true;
}

/*!
    \fn QWebMethod::init()

    Performs genral initialisation of the object. Sets default variable values, initializes
    network manager, connects reply signals. Calls virtual method, configure().
  */
void QWebMethod::init()
{
    m_username = "";
    m_password = "";

    replyReceived = false;
    errorState = false;
    authenticationError = false;
    errorMessage = "";

    manager = new QNetworkAccessManager(this);

    reply.clear();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    connect(manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this, SLOT(authenticate(QNetworkReply*,QAuthenticator*)));
}

/*!
    \fn QWebMethod::prepareRequestData()

    Protected function, invoked by sendMessage(). Modifies QByteArray data, so that it is consistent with
    protocol and HTTP method specification. It uses QMap<QString, QVariant> parameters to fill data object's body.
    Can be overriden by creating custom QByteArray and passing it to sendMessage().

    \sa sendMessage()
  */
void QWebMethod::prepareRequestData()
{
    data.clear();
    QString header, body, footer;
    QString endl = "\r\n"; // Replace with something OS-independent, or seriously rethink.

    if (protocolUsed & soap) {
        if (protocolUsed & soap12) {
            header = "<?xml version=\"1.0\" encoding=\"utf-8\"?> " + endl +
                     " <soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " +
                     "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" " +
                     "xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\"> " + endl +
                     " <soap12:Body> " + endl;

            footer = "</soap12:Body> " + endl + "</soap12:Envelope>";
        }
        else if (protocolUsed & soap10) {
            header = "<?xml version=\"1.0\" encoding=\"utf-8\"?> " + endl +
                    " <soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " +
                    "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" " +
                    "xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\"> " + endl +
                    " <soap:Body> " + endl;

            footer = "</soap:Body> " + endl + "</soap:Envelope>";
        }

        body = "\t<" + m_messageName + " xmlns=\"" + m_targetNamespace + "\"> " + endl;

        foreach (const QString currentKey, parameters.keys()) {
            QVariant qv = parameters.value(currentKey);
            // Currently, this does not handle nested lists
            body += "\t\t<" + currentKey + ">" + qv.toString() + "</" + currentKey + "> " + endl;
        }

        body += "\t</" + m_messageName + "> " + endl;
    }
    else if (protocolUsed & http) {
        foreach (const QString currentKey, parameters.keys()) {
            QVariant qv = parameters.value(currentKey);
            // Currently, this does not handle nested lists
            body += currentKey + "=" + qv.toString() + "&";
        }
        body.chop(1);
    }
    else if (protocolUsed & json) {
        body += "{" + endl;
        foreach (const QString currentKey, parameters.keys()) {
            QVariant qv = parameters.value(currentKey);
            // Currently, this does not handle nested lists
            body += "{" + endl + "\t\"" + currentKey + "\" : \"" + qv.toString() + "\"" + endl;
        }
        body += "}";
    }
    else if (protocolUsed & xml) {
        foreach (const QString currentKey, parameters.keys()) {
            QVariant qv = parameters.value(currentKey);
            // Currently, this does not handle nested lists
            body += "\t\t<" + currentKey + ">" + qv.toString() + "</" + currentKey + "> " + endl;
        }
    }

    data.append(header + body + footer);
}

/*!
    \internal
    \fn QWebMethod::convertReplyToUtf(QString textToConvert)

    Changes the encoding of the reply, in a rather crude fashion.
  */
QString QWebMethod::convertReplyToUtf(QString textToConvert)
{
    QString result = textToConvert;

    result.replace("&lt;", "<");
    result.replace("&gt;", ">");

    return result;
}

/*!
    \internal
    \fn QWebMethod::enterErrorState(QString errMessage)

    Enters into error state with message \a errMessage.
  */
bool QWebMethod::enterErrorState(QString errMessage)
{
    errorState = true;
    errorMessage += errMessage + " ";
    qDebug() << errMessage;
    emit errorEncountered(errMessage);
    return false;
}
