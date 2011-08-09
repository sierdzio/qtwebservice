#include "../headers/qsoapmessage.h"

/*!
    \class QSoapMessage
    \brief Class that can be used to asnchronously and synchronously send HTTP, SOAP1.0 and SOAP1.2 messages to web services.

    To send a message and receive reply synchronously, use the static sendMessage() method. Otherwise, you can use
    replyReady() signal to know, when a reply returns. It can be read with replyRead().
  */

/*!
     \enum QSoapMessage::Protocol

     This enum type specifies the protocol that QSoapMessage will use in communicating with the server:

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
 */

/*!
    \fn QSoapMessage::QSoapMessage(QObject *parent)

    Constructs the message usign \a parent. Requires specifying other params later (setParams()).

    \sa setParams(), setProtocol(), sendMessage()
  */
QSoapMessage::QSoapMessage(QObject *parent) :
    QObject(parent)
{
    init();
    host = "";
    hostUrl.setHost(host);
    messageName = "";
    parameters.clear();
}

/*!
    \fn QSoapMessage::QSoapMessage(QUrl url, QString _messageName, QObject *parent)

    Constructs the message using \a url, \a _messageName, and \a parent.
    Requires params to be specified later.

    \sa setParams(), setProtocol(), sendMessage()
  */
QSoapMessage::QSoapMessage(QUrl url, QString _messageName, QObject *parent) :
    QObject(parent), hostUrl(url), messageName(_messageName)
{
    init();
    host = hostUrl.host();
    parameters.clear();
}

/*!
    \fn QSoapMessage::QSoapMessage(QString url, QString _messageName, QObject *parent)

    Constructs the message using \a url, \a _messageName, and \a parent.
    Requires params to be specified later.

    \sa setParams(), setProtocol(), sendMessage()
  */
QSoapMessage::QSoapMessage(QString url, QString _messageName, QObject *parent) :
    QObject(parent), host(url), messageName(_messageName)
{
    init();
    hostUrl.setHost(host + messageName);
    parameters.clear();
}

/*!
    \fn QSoapMessage::QSoapMessage(QString url, QString _messageName, QMap<QString, QVariant> params, QObject *parent)

    Constructs the message using \a url, \a _messageName, and \a parent. This constructor also takes
    message parameters (\a params).
    Does not require specifying any more information, but you still need to manually send the message
    using sendMessage() (without any arguments, or else - if you want to change ones specified here).

    \sa sendMessage(), setProtocol()
  */
QSoapMessage::QSoapMessage(QString url, QString _messageName,
                           QMap<QString, QVariant> params, QObject *parent) :
    QObject(parent), host(url), messageName(_messageName), parameters(params)
{
    init();
    hostUrl.setHost(host + messageName);
}

/*!
    \fn QSoapMessage::~QSoapMessage()

    Deletes internal pointers.
  */
QSoapMessage::~QSoapMessage()
{
    delete manager;
    delete networkReply;
    this->deleteLater();
}

/*!
    \fn QSoapMessage::setParams(QMap<QString, QVariant> params)

    Sets method's parameters (\a params). This also includes their names.
  */
void QSoapMessage::setParams(QMap<QString, QVariant> params)
{
    parameters = params;
}

/*!
    \fn QSoapMessage::setReturnValue(QMap<QString, QVariant> returnVal)

    Sets method's return value (\a returnVal). This also includes their names.
  */
void QSoapMessage::setReturnValue(QMap<QString, QVariant> returnVal)
{
    returnValue = returnVal;
}

/*!
    \fn QSoapMessage::setTargetNamespace(QString tNamespace)

    Sets message's target namespace (\a tNamespace), which is needed in SOAP messaging.
  */
void QSoapMessage::setTargetNamespace(QString tNamespace)
{
    targetNamespace = tNamespace;
}

/*!
    \fn QSoapMessage::setProtocol(Protocol prot)

    Sets the protocol flag (\a prot, being one of the values of QSoapMessage::Protocol).
    This determines the protocol used later, when sending request. Defaults to SOAP1.2
  */
void QSoapMessage::setProtocol(Protocol prot)
{
    if (prot == soap)
        protocol = soap12;
    else
        protocol = prot;
}

/*!
    \fn QSoapMessage::sendMessage()

    Sends the message asynchronously, assuming that all neccessary data was specified earlier.
    Returns true on success.

    \sa setParams(), setProtocol(), setTargetNamespace()
  */
bool QSoapMessage::sendMessage()
{
    hostUrl.setUrl(host);
    QNetworkRequest request;
    request.setUrl(hostUrl);
    if (protocol & soap)
        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/soap+xml; charset=utf-8"));
    else if (protocol == json)
        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json; charset=utf-8"));
    else if (protocol == http)
        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("Content-Type: application/x-www-form-urlencoded"));

    if (protocol == soap10)
        request.setRawHeader(QByteArray("SOAPAction"), QByteArray(hostUrl.toString().toAscii()));

    prepareRequestData();

    //qDebug() << request.rawHeaderList() << " " << request.url().toString();
    //qDebug() << data;

    manager->post(request, data);
    return true;
}

/*!
    \fn QSoapMessage::sendMessage(QMap<QString, QVariant> params)

    Sends the message asynchronously using parameters specified in \a params.
  */
bool QSoapMessage::sendMessage(QMap<QString, QVariant> params)
{
    parameters = params;
    sendMessage();
    return true;
}

/*!
     STATIC method. Sends the message synchronously, using \a url, \a _messageName, \a params and \a parent.
     Returns with web service reply.
  */
QVariant QSoapMessage::sendMessage(QObject *parent, QUrl url, QString _messageName, QMap<QString, QVariant> params)
{
    QSoapMessage qsm(url.host(), _messageName, params, parent);
    qsm.hostUrl = url;

    qsm.sendMessage();
    // TODO: ADD ERROR HANDLING!
    forever
    {
        if (qsm.replyReceived)
            return qsm.reply;
        else
        {
            qApp->processEvents();
        }
    }
}

/*!
    \fn QSoapMessage::replyRead()

    After making asynchronous call, and getting the replyReady() signal, this method can be used to read the reply.
  */
QVariant QSoapMessage::replyRead()
{
    return reply;
}

/*!
    \fn QSoapMessage::replyReady(QVariant rply)

    Signal invoked when the reply (\a rply) from web service's server is ready for reading.
  */

/*!
    \fn QSoapMessage::getMessageName()

    Returns message's name.
  */
QString QSoapMessage::getMessageName()
{
    return messageName;
}

/*!
    \fn QSoapMessage::getParameterNames() const

    Retrurns list of parameters' names.
  */
QStringList QSoapMessage::getParameterNames() const
{
    return (QStringList) parameters.keys();
}

/*!
    \fn QSoapMessage::getReturnValueName() const

    Returns return value's name.
  */
QStringList QSoapMessage::getReturnValueName() const
{
    return (QStringList) returnValue.keys();
}

/*!
    \fn QSoapMessage::getParameterNamesTypes() const

    Returns whole parameter information (name and type).
  */
QMap<QString, QVariant> QSoapMessage::getParameterNamesTypes() const
{
    return parameters;
}

/*!
    \fn QSoapMessage::getReturnValueNameType() const

    Returns whole return value information (name and type).
  */
QMap<QString, QVariant> QSoapMessage::getReturnValueNameType() const
{
    return returnValue;
}

/*!
    \fn QSoapMessage::getTargetNamespace()

    Returns target namespace.
  */
QString QSoapMessage::getTargetNamespace()
{
    return targetNamespace;
}

/*!
    \fn QSoapMessage::replyFinished(QNetworkReply *netReply)

    Public (will probably be private in the future) slot, which processes the reply (\a netReply) from the server.
    Emits the replyReady() signal.
  */
void QSoapMessage::replyFinished(QNetworkReply *netReply)
{
    networkReply = netReply;
    QByteArray replyBytes;

    replyBytes = (networkReply->readAll());
    QString replyString = convertReplyToUtf(replyBytes);

    QString tempBegin = "<" + messageName + "Result>";
    int replyBeginIndex = replyString.indexOf(tempBegin, 0, Qt::CaseSensitive);
    replyBeginIndex += tempBegin.length();

    QString tempFinish = "</" + messageName + "Result>";
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
    \internal
    \fn QSoapMessage::init()
  */
void QSoapMessage::init()
{
    protocol = soap12;
    replyReceived = false;

    manager = new QNetworkAccessManager(this);

    reply.clear();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

/*!
    \internal
    \fn QSoapMessage::prepareRequestData()
  */
void QSoapMessage::prepareRequestData()
{
    data.clear();
    QString header, body, footer;
    QString endl = "\r\n"; // Replace with something OS-independent, or seriously rethink.

    if (protocol & soap)
    {
        if (protocol == soap12)
        {
            header = "<?xml version=\"1.0\" encoding=\"utf-8\"?> " + endl +
                     " <soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" +
                     "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"" +
                     "xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\"> " + endl +
                     " <soap12:Body> " + endl;

            footer = "</soap12:Body> " + endl + "</soap12:Envelope>";
        }
        else if (protocol == soap10)
        {
            header = "<?xml version=\"1.0\" encoding=\"utf-8\"?> " + endl +
                    " <soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" +
                    "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"" +
                    "xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\"> " + endl +
                    " <soap:Body> " + endl;

            footer = "</soap:Body> " + endl + "</soap:Envelope>";
        }

        body = "\t<" + messageName + " xmlns=\"" + targetNamespace + "\"> " + endl;

        foreach (const QString currentKey, parameters.keys())
        {
            QVariant qv = parameters.value(currentKey);
            // Currently, this does not handle nested lists
            body += "\t\t<" + currentKey + ">" + qv.toString() + "</" + currentKey + "> " + endl;
        }

        body += "\t</" + messageName + "> " + endl;
    }
    else if (protocol == http)
    {
        foreach (const QString currentKey, parameters.keys())
        {
            QVariant qv = parameters.value(currentKey);
            // Currently, this does not handle nested lists
            body += currentKey + "=" + qv.toString() + "&";
        }
        body.chop(1);
    }
    else if (protocol == json)
    {
        body += "{" + endl;
        foreach (const QString currentKey, parameters.keys())
        {
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
    \fn QSoapMessage::convertReplyToUtf(QString textToConvert)

    Changes the encoding of the reply, in a rather crude fashion.
  */
QString QSoapMessage::convertReplyToUtf(QString textToConvert)
{
    QString result = textToConvert;

    result.replace("&lt;", "<");
    result.replace("&gt;", ">");

    return result;
}
