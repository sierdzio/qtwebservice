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
 */

/*!
     \enum QSoapMessage::Role

     This enum type specifies message's role. Currently it is purely internal and NOT USED:

     \value outboundRole
            Obsolete part, from older implementation.
     \value inboundRole
            Obsolete part, from older implementation.
     \value staticRole
            Set when used in a static way.
     \value noRole
            Obsolete part, from older implementation.
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
    hostname = "";
    hostUrl.setHost(hostname);
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
    hostname = hostUrl.host();
    parameters.clear();
}

/*!
    \fn QSoapMessage::QSoapMessage(QString url, QString _messageName, QObject *parent)

    Constructs the message using \a url, \a _messageName, and \a parent.
    Requires params to be specified later.

    \sa setParams(), setProtocol(), sendMessage()
  */
QSoapMessage::QSoapMessage(QString url, QString _messageName, QObject *parent) :
    QObject(parent), hostname(url), messageName(_messageName)
{
    init();
    hostUrl.setHost(hostname + messageName);
    parameters.clear();
}

/*!
    \fn QSoapMessage::QSoapMessage(QString url, QString _messageName, QMap<QString, QVariant> params, QMap<QString, QVariant> returnVal, QObject *parent)

    Constructs the message using \a url, \a _messageName, and \a parent. This constructor also takes
    message parameters (\a params) and return value (\a returnVal).
    Does not require specifying any more information, but you still need to manually send the message
    using sendMessage() (without any arguments, or else - if you want to change ones specified here).

    \sa sendMessage(), setProtocol()
  */
QSoapMessage::QSoapMessage(QString url, QString _messageName,
                           QMap<QString, QVariant> params, QMap<QString, QVariant> returnVal, QObject *parent) :
    QObject(parent), hostname(url), messageName(_messageName), parameters(params), returnValue(returnVal)
{
    init();
    hostUrl.setHost(hostname + messageName);
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
    \fn QSoapMessage::setParams(QMap<QString, QVariant> params, QMap<QString, QVariant> returnVal)

    Sets method's parameters (\a params) and return value (\a returnVal). This also includes their names
    (that's why you need to specify the return value).
  */
void QSoapMessage::setParams(QMap<QString, QVariant> params, QMap<QString, QVariant> returnVal)
{
    parameters = params;
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
    hostUrl.setUrl(hostname);
    QNetworkRequest request;
    request.setUrl(hostUrl); //"http://www.webserviceX.NET/stockquote.asmx"));
    //request.setHeader(QNetworkRequest::LocationHeader, QVariant("/stockquote.asmx"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/soap+xml; charset=utf-8"));
    if (protocol == soap10)
        request.setRawHeader(QByteArray("SOAPAction"), QByteArray(hostUrl.toString().toAscii()));

    prepareRequestData();

    //qDebug() << request.rawHeaderList() << " " << request.url().toString();
    //qDebug() << data;
    //qDebug() << "*************************";

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
     STATIC method. Sends the message synchronously, using \a url, \a _messageName, \a params, \a returnVal and \a parent.
     Returns with web service reply.
  */
QVariant QSoapMessage::sendMessage(QObject *parent, QUrl url, QString _messageName, QMap<QString, QVariant> params,
                                   QMap<QString, QVariant> returnVal)
{
    QSoapMessage qsm(url.host(), _messageName, params, returnVal, parent);
    qsm.role = staticRole;
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
            //sleep(2);
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
    //networkReply = new QNetworkReply(this);
    //data = new QByteArray();

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

    if (protocol == soap12)
    {
        header = "<?xml version=\"1.0\" encoding=\"utf-8\"?> \r\n <soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\"> \r\n <soap12:Body> \r\n";

        footer = "</soap12:Body> \r\n</soap12:Envelope>";
    }

    body = "\t<" + messageName + " xmlns=\"" + targetNamespace + "\"> \r\n";

    foreach (const QString currentKey, parameters.keys())
    {
        QVariant qv = parameters.value(currentKey);
        // Currently, this does not handle nested lists
        body += "\t\t<" + currentKey + ">" + qv.toString() + "</" + currentKey + "> \r\n";
    }

    body += "\t</" + messageName + "> \r\n";

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
