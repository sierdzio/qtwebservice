#include "../headers/qsoapmessage.h"

QSoapMessage::QSoapMessage(QObject *parent) :
    QObject(parent)
{
    init();
    hostname = "";
    hostUrl.setHost(hostname);
    messageName = "";
    parameters.clear();
}

QSoapMessage::QSoapMessage(QUrl url, QString _messageName, QObject *parent) :
    QObject(parent), hostUrl(url), messageName(_messageName)
{
    init();
    hostname = hostUrl.host();
    parameters.clear();
}

QSoapMessage::QSoapMessage(QString url, QString _messageName, QObject *parent) :
    QObject(parent), hostname(url), messageName(_messageName)
{
    init();
    hostUrl.setHost(hostname + messageName);
    parameters.clear();
}

QSoapMessage::QSoapMessage(QString url, QString _messageName,
                           QMap<QString, QVariant> params, QMap<QString, QVariant> returnVal, QObject *parent) :
    QObject(parent), hostname(url), messageName(_messageName), parameters(params), returnValue(returnVal)
{
    init();
    hostUrl.setHost(hostname + messageName);
}

QSoapMessage::~QSoapMessage()
{
    delete manager;
    delete networkReply;
    //delete data;
    this->deleteLater();
}

void QSoapMessage::setParams(QMap<QString, QVariant> params, QMap<QString, QVariant> returnVal)
{
    parameters = params;
    returnValue = returnVal;
}

void QSoapMessage::setTargetNamespace(QString tNamespace)
{
    targetNamespace = tNamespace;
}

bool QSoapMessage::sendMessage()
{
    hostUrl.setUrl(hostname);
    QNetworkRequest request;
    request.setUrl(hostUrl); //"http://www.webserviceX.NET/stockquote.asmx"));
    //request.setHeader(QNetworkRequest::LocationHeader, QVariant("/stockquote.asmx"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/soap+xml; charset=utf-8"));
    if (version == soap10)
        request.setRawHeader(QByteArray("SOAPAction"), QByteArray("\"http://www.webserviceX.NET/GetQuote\""));

    prepareRequestData();

    qDebug() << request.rawHeaderList() << " " << request.url().toString();
    //qDebug() << data;
    qDebug() << "*************************";

    manager->post(request, data);
}

bool QSoapMessage::sendMessage(QMap<QString, QVariant> params)
{
    parameters = params;
    sendMessage();
}

/* STATIC */
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

QVariant QSoapMessage::replyRead()
{
    return reply;
}

QStringList QSoapMessage::getParameterNames()
{
    return (QStringList) parameters.keys();
}

QStringList QSoapMessage::getReturnValueName()
{
    return (QStringList) returnValue.keys();
}

QMap<QString, QVariant> QSoapMessage::getParameterNamesTypes()
{
    return parameters;
}

QMap<QString, QVariant> QSoapMessage::getReturnValueNameType()
{
    return returnValue;
}

QString QSoapMessage::getTargetNamespace()
{
    return targetNamespace;
}

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

void QSoapMessage::init()
{
    version = soap12;
    replyReceived = false;

    manager = new QNetworkAccessManager(this);
    //networkReply = new QNetworkReply(this);
    //data = new QByteArray();

    reply.clear();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void QSoapMessage::prepareRequestData()
{
    data.clear();
    QString header, body, footer;

    if (version == soap12)
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

QString QSoapMessage::convertReplyToUtf(QString textToConvert)
{
    QString result = textToConvert;

    result.replace("&lt;", "<");
    result.replace("&gt;", ">");

    return result;
}

//TEMP
bool QSoapMessage::isReplyReceived()
{
    return replyReceived;
}
