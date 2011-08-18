#include "../headers/qwebmethod.h"

/*!
    \class QWebMethod
    \brief Class that can be used to asnchronously and synchronously send HTTP, SOAP1.0 and SOAP1.2 messages to web services.

    To send a message and receive reply synchronously, use the static sendMessage() method. Otherwise, you can use
    replyReady() signal to know, when a reply returns. It can be read with replyRead().

    If you want to save some time on configuration in your code, you can subclass QWebMethod, reimplement configure(),
    and create your own slot for parsing te reply. Configure is called by init(), which is in turn called by every
    constructor.

    You can use configure() to:
    \list
        \o set host Url
        \o set protocol
        \o set message name
        \o set target namespace
        \o connect replyReady() signal to your custom slot (where you can parse the reply
           and return it in a type of your convenience)
    \endlist

    \sa init(), configure()
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

  Defines HTTP method to use when sending the message.

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
    \fn QWebMethod::QWebMethod(QObject *parent, Protocol protocol)

    Constructs the message usign \a parent and \a protocol (which defaults to soap12). Requires specifying other params later (setParameters()).

    \sa init(), setParameters(), setProtocol(), sendMessage()
  */
QWebMethod::QWebMethod(QObject *parent, Protocol protocol) :
    QObject(parent), protocolUsed(protocol)
{
    init();
    m_hostUrl.setHost("");
    m_messageName = "";
    parameters.clear();
}

/*!
    \fn QWebMethod::QWebMethod(QUrl url, QString messageName, QObject *parent, Protocol protocol)

    Constructs the message using \a url, \a messageName, \a parent and \a protocol (which defaults to soap12).
    Requires params to be specified later.

    \sa init(), setParameters(), setProtocol(), sendMessage()
  */
QWebMethod::QWebMethod(QUrl url, QString messageName, QObject *parent, Protocol protocol) :
    QObject(parent), m_hostUrl(url), m_messageName(messageName), protocolUsed(protocol)
{
    init();
    parameters.clear();
}

/*!
    \fn QWebMethod::QWebMethod(QString url, QString messageName, QObject *parent, Protocol protocol)

    Constructs the message using \a url, \a messageName, \a parent and \a protocol (which defaults to soap12).
    Requires params to be specified later.

    \sa init(), setParameters(), setProtocol(), sendMessage()
  */
QWebMethod::QWebMethod(QString url, QString messageName, QObject *parent, Protocol protocol) :
    QObject(parent), m_messageName(messageName), protocolUsed(protocol)
{
    init();
    m_hostUrl.setHost(url + m_messageName);
    parameters.clear();
}

/*!
    \fn QWebMethod::QWebMethod(QString url, QString messageName, QMap<QString, QVariant> params, QObject *parent, Protocol protocol)

    Constructs the message using \a url, \a messageName, \a parent and \a protocol (which defaults to soap12).
    This constructor also takes message parameters (\a params).
    Does not require specifying any more information, but you still need to manually send the message
    using sendMessage() (without any arguments, or else - if you want to change ones specified here).

    \sa init(), sendMessage(), setProtocol()
  */
QWebMethod::QWebMethod(QString url, QString messageName,
                           QMap<QString, QVariant> params, QObject *parent, Protocol protocol) :
    QObject(parent), m_messageName(messageName), parameters(params), protocolUsed(protocol)
{
    init();
    m_hostUrl.setHost(url + m_messageName);
}

/*!
    \fn QWebMethod::~QWebMethod()

    Deletes internal pointers.
  */
QWebMethod::~QWebMethod()
{
    delete manager;
    delete networkReply;
    this->deleteLater();
}

/*!
  \fn QWebMethod::setHost(QString newHost)

  Set's message's host to \a newHost.
  */
void QWebMethod::setHost(QString newHost)
{
    m_hostUrl.setHost(newHost);
}

/*!
  \fn QWebMethod::setHost(QUrl newHost)

  Set's message's host to \a newHost.
  */
void QWebMethod::setHost(QUrl newHost)
{
    m_hostUrl = newHost;
}

/*!
  \fn QWebMethod::setMessageName(QString newName)

  Set's message's name to \a newName.
  */
void QWebMethod::setMessageName(QString newName)
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
    This determines the protocol used later, when sending request. Defaults to SOAP1.2
  */
void QWebMethod::setProtocol(Protocol prot)
{
    if (prot == soap)
        protocolUsed = soap12;
    else
        protocolUsed = prot;
}

/*!
    \fn QWebMethod::sendMessage()

    Sends the message asynchronously, assuming that all neccessary data was specified earlier.
    Returns true on success.

    \sa setParameters(), setProtocol(), setTargetNamespace()
  */
bool QWebMethod::sendMessage()
{
    QNetworkRequest request;
    request.setUrl(m_hostUrl);

    if (protocolUsed & soap)
        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/soap+xml; charset=utf-8"));
    else if (protocolUsed == json)
        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json; charset=utf-8"));
    else if (protocolUsed == http)
        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("Content-Type: application/x-www-form-urlencoded"));

    if (protocolUsed == soap10)
        request.setRawHeader(QByteArray("SOAPAction"), QByteArray(m_hostUrl.toString().toAscii()));

    prepareRequestData();

    //qDebug() << request.rawHeaderList() << " " << request.url().toString();
    //qDebug() << data;

    manager->post(request, data);
    return true;
}

/*!
    \fn QWebMethod::sendMessage(QMap<QString, QVariant> params)

    Sends the message asynchronously using parameters specified in \a params.
  */
bool QWebMethod::sendMessage(QMap<QString, QVariant> params)
{
    parameters = params;
    sendMessage();
    return true;
}

/*!
     STATIC method. Sends the message synchronously, using \a url, \a _messageName, \a params and \a parent.
     Protocol can optionally be specified by \a protocol.
     Returns with web service reply.
  */
QVariant QWebMethod::sendMessage(QObject *parent, QUrl url, QString _messageName,
                                 QMap<QString, QVariant> params, Protocol protocol)
{
    QWebMethod qsm(url.host(), _messageName, params, parent);
    qsm.m_hostUrl = url;
    qsm.protocolUsed = protocol;

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
    \fn QWebMethod::messageName()

    Returns message's name.
  */
QString QWebMethod::messageName()
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
    \fn QWebMethod::targetNamespace()

    Returns target namespace.
  */
QString QWebMethod::targetNamespace()
{
    return m_targetNamespace;
}

/*!
    \fn QWebMethod::host()

    Returns host's URL (in QString). If you want a QUrl, call getHostUrl(), or QUrl(QWebMethod::getHost());
  */
QString QWebMethod::host()
{
    return m_hostUrl.host();
}

/*!
    \fn QWebMethod::hostUrl()

    Returns host's URL. If you want a QString, call getHost() or getHostUrl().host();
  */
QUrl QWebMethod::hostUrl()
{
    return m_hostUrl;
}

/*!
    \fn QWebMethod::replyFinished(QNetworkReply *netReply)

    Public (will probably be private in the future) slot, which processes the reply (\a netReply) from the server.
    Emits the replyReady() signal.
  */
void QWebMethod::replyFinished(QNetworkReply *netReply)
{
    networkReply = netReply;
    QByteArray replyBytes;

    replyBytes = (networkReply->readAll());
    QString replyString = convertReplyToUtf(replyBytes);

    QString tempBegin = "<" + m_messageName + "Result>";
    int replyBeginIndex = replyString.indexOf(tempBegin, 0, Qt::CaseSensitive);
    replyBeginIndex += tempBegin.length();

    QString tempFinish = "</" + m_messageName + "Result>";
    int replyFinishIndex = replyString.indexOf(tempFinish, replyBeginIndex, Qt::CaseSensitive);

    if (replyBeginIndex == -1)
        replyBytes = 0;
    if (replyFinishIndex == -1)
        replyFinishIndex = replyString.length();

    reply = (QVariant) replyString.mid(replyBeginIndex, replyFinishIndex - replyBeginIndex);

    replyReceived = true;
    emit replyReady(reply);
}

/*!
    \fn QWebMethod::init()

    Performs genral initialisation of the object. Sets default variable values, initializes
    network manager, connects reply signals. Calls virtual method, configure().

    \sa configure()
  */
void QWebMethod::init()
{
    replyReceived = false;

    manager = new QNetworkAccessManager(this);

    reply.clear();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    configure();
}

/*!
  \fn QWebMethod::configure()

  Vitrual method. Itis called by QWebMethod's constructor (or, more precisely, by init()).

  You can use it to:
  \list
    \o set host Url using setHost() or setHostUrl()
    \o set protocol using setProtocol()
    \o set message name using setMesageName()
    \o set target namespace using setTargetNamespace()
    \o connect replyReady() signal to your custom slot (where you can parse the reply and return it in a type of your convenience)
  \endlist

  \sa init()
  */
void QWebMethod::configure()
{
}

/*!
    \internal
    \fn QWebMethod::prepareRequestData()
  */
void QWebMethod::prepareRequestData()
{
    data.clear();
    QString header, body, footer;
    QString endl = "\r\n"; // Replace with something OS-independent, or seriously rethink.

    if (protocolUsed & soap) {
        if (protocolUsed == soap12) {
            header = "<?xml version=\"1.0\" encoding=\"utf-8\"?> " + endl +
                     " <soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" +
                     "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"" +
                     "xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\"> " + endl +
                     " <soap12:Body> " + endl;

            footer = "</soap12:Body> " + endl + "</soap12:Envelope>";
        }
        else if (protocolUsed == soap10) {
            header = "<?xml version=\"1.0\" encoding=\"utf-8\"?> " + endl +
                    " <soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" +
                    "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"" +
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
    else if (protocolUsed == http) {
        foreach (const QString currentKey, parameters.keys()) {
            QVariant qv = parameters.value(currentKey);
            // Currently, this does not handle nested lists
            body += currentKey + "=" + qv.toString() + "&";
        }
        body.chop(1);
    }
    else if (protocolUsed == json) {
        body += "{" + endl;
        foreach (const QString currentKey, parameters.keys()) {
            QVariant qv = parameters.value(currentKey);
            // Currently, this does not handle nested lists
            body += "{" + endl + "\t\"" + currentKey + "\" : \"" + qv.toString() + "\"" + endl;
        }
        body += "}";
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
