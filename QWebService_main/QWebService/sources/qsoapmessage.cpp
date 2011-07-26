#include "../headers/qsoapmessage.h"

/*!
    \class QSoapMessage
    \brief Class that can be used to asnchronously and synchronously send HTTP, SOAP1.0 and SOAP1.2 messages to web services.

    To send a message and receive reply synchronously, use the static sendMessage() method. Otherwise, you can use
    replyReady() signal to know, when a reply returns. It can be read with replyRead().
  */

/*!
    \fn QSoapMessage::QSoapMessage(QObject *parent)

    Constructs the message usign \a parent. Requires specifying other params late (setParams()).
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
  */
QSoapMessage::~QSoapMessage()
{
    delete manager;
    delete networkReply;
    //delete data;
    this->deleteLater();
}

/*!
    \fn QSoapMessage::setParams(QMap<QString, QVariant> params, QMap<QString, QVariant> returnVal)
  */
void QSoapMessage::setParams(QMap<QString, QVariant> params, QMap<QString, QVariant> returnVal)
{
    parameters = params;
    returnValue = returnVal;
}

/*!
    \fn QSoapMessage::setTargetNamespace(QString tNamespace)
  */
void QSoapMessage::setTargetNamespace(QString tNamespace)
{
    targetNamespace = tNamespace;
}

/*!
    \fn QSoapMessage::setProtocol(Protocol prot)
  */
void QSoapMessage::setProtocol(Protocol prot)
{
    protocol = prot;
}

/*!
    \fn QSoapMessage::sendMessage()
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
  */
bool QSoapMessage::sendMessage(QMap<QString, QVariant> params)
{
    parameters = params;
    sendMessage();
    return true;
}

/*!
    \fn QSoapMessage::sendMessage(QObject *parent, QUrl url, QString _messageName, QMap<QString, QVariant> params, QMap<QString, QVariant> returnVal)

     STATIC method.
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
  */
QVariant QSoapMessage::replyRead()
{
    return reply;
}

/*!
    \fn QSoapMessage::getMessageName()
  */
QString QSoapMessage::getMessageName()
{
    return messageName;
}

/*!
    \fn QSoapMessage::getParameterNames() const
  */
QStringList QSoapMessage::getParameterNames() const
{
    return (QStringList) parameters.keys();
}

/*!
    \fn QSoapMessage::getReturnValueName() const
  */
QStringList QSoapMessage::getReturnValueName() const
{
    return (QStringList) returnValue.keys();
}

/*!
    \fn QSoapMessage::getParameterNamesTypes() const
  */
QMap<QString, QVariant> QSoapMessage::getParameterNamesTypes() const
{
    return parameters;
}

/*!
    \fn QSoapMessage::getReturnValueNameType()
  */
QMap<QString, QVariant> QSoapMessage::getReturnValueNameType() const
{
    return returnValue;
}

/*!
    \fn QSoapMessage::getTargetNamespace()
  */
QString QSoapMessage::getTargetNamespace()
{
    return targetNamespace;
}

/*!
    \fn QSoapMessage::replyFinished(QNetworkReply *netReply)
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
    \fn QSoapMessage::convertReplyToUtf(QString textToConvert)
  */
QString QSoapMessage::convertReplyToUtf(QString textToConvert)
{
    QString result = textToConvert;

    result.replace("&lt;", "<");
    result.replace("&gt;", ">");

    return result;
}
