#include "../headers/standardpath.h"

StandardPath::StandardPath(QObject *parent) :
    QObject(parent)
{
}

void StandardPath::prepare()
{
    workingDir.mkdir("headers");
    workingDir.mkdir("sources");

    messages = wsdl->getMethods();
}

bool StandardPath::create(QWsdl *w, QDir wrkDir, QObject *parent)
{
    StandardPath obj(parent);
    obj.workingDir = wrkDir;
    obj.wsdl = w;
    obj.prepare();

    if (!obj.createMessages())
        return false;
    return true;
}

bool StandardPath::createMessages()
{
    workingDir.cd("headers");

    foreach (QString s, messages->keys())
    {
        QSoapMessage *m = messages->value(s);
        createMessageHeader(m);
//        delete m;
    }

    workingDir.cdUp();
    workingDir.cd("sources");

    foreach (QString s, messages->keys())
    {
        QSoapMessage *n = messages->value(s);
        createMessageSource(n);
//        delete n;
    }

    workingDir.cdUp();
    return true;
}

bool StandardPath::createMessageHeader(QSoapMessage *msg)
{
    QString msgName = msg->getMessageName();
    QString msgReplyName = msg->getReturnValueName().first(); // Possible problem in case of multi-return.

    QMap<QString, QVariant> tempMap = msg->getReturnValueNameType();
    QString msgReplyType = "";
    foreach (QString s, tempMap.keys())
    {
        msgReplyType += tempMap.value(s).typeName();
        break;
    }

    tempMap.clear();
    tempMap = msg->getParameterNamesTypes();
    QString msgParameters = "";
    foreach (QString s, tempMap.keys())
    {
        msgParameters += QString(tempMap.value(s).typeName()) + " " + s + ",";
    }
    msgParameters.chop(1);


    QFile file(workingDir.path() + "/" + msgName + ".h");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return false;

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "#ifndef Q" << msgName.toUpper() << endl; // might break on curious names
    out << "#define Q" << msgName.toUpper() << endl;
    out << endl;
    out << "#include <QtCore>" << endl;
    out << "#include <QNetworkAccessManager>" << endl;
    out << "#include <QNetworkRequest>" << endl;
    out << "#include <QNetworkReply>" << endl;
    out << endl;
    out << "class " << msgName << " : public QObject" << endl;
    out << "{" << endl;
    out << "    Q_OBJECT" << endl;
    out << endl;
    out << "public:" << endl;
    out << "    enum Role {outboundRole, inboundRole, staticRole, noRole};" << endl;
    out << "    enum Protocol {http, soap10, soap12};" << endl;
    out << endl;
    out << "    explicit " << msgName << "(QObject *parent = 0);" << endl;
//    out << "    QSoapMessage(QUrl hostUrl, QString messageName, QObject *parent = 0);" << endl;
//    out << "    QSoapMessage(QString hostname, QString messageName, QObject *parent = 0);" << endl;
//    out << "    QSoapMessage(QString hostname, QString messageName, QMap<QString, QVariant> params," << endl;
//    out << "                 QMap<QString, QVariant> returnValue, QObject *parent = 0);" << endl;
    out << "    ~" << msgName << "();" << endl;
    out << endl;
    out << "    void setParams(" << msgParameters << ");" << endl; // << ", " << msgReplyType << " " << msgReplyName << ");" << endl;
//    out << "    void setTargetNamespace(QString tNamespace);" << endl;
    out << "    void setProtocol(Protocol protocol);" << endl;
    out << "    bool sendMessage();" << endl;
    out << "    bool sendMessage(" << msgParameters << ");" << endl;
    out << "    " << msgReplyType << " static sendMessage(QObject *parent, QUrl url, QString _messageName," << endl;
    out << "                                " << msgParameters << ");" << endl; //", " << msgReplyType << " " << msgReplyName << ");" << endl;
    out << "    " << msgReplyType << " replyRead();" << endl;
    out << "    QString getMessageName();" << endl;
    out << "    QStringList getParameterNames() const;" << endl;
    out << "    QString getReturnValueName() const;" << endl;
    out << "    QMap<QString, QVariant> getParameterNamesTypes() const;" << endl;
    out << "    QString getReturnValueNameType() const;" << endl;
    out << "    QString getTargetNamespace();" << endl;
    out << endl;
    out << "signals:" << endl;
    out << "    void replyReady(" << msgReplyType << " " << msgReplyName << ");" << endl;
    out << endl;
    out << "public slots:" << endl;
    out << "    void replyFinished(QNetworkReply *reply);" << endl;
    out << endl;
    out << "private:" << endl;
    out << "    void init();" << endl;
    out << "    void prepareRequestData();" << endl;
    out << "    QString convertReplyToUtf(QString textToConvert);" << endl;
    out << endl;
    out << "    bool replyReceived;" << endl;
    out << "    Role role;" << endl;
    out << "    Protocol protocol;" << endl;
    out << "    QUrl hostUrl;" << endl;
    out << "    QString hostname;" << endl;
    out << "    QString messageName;" << endl;
    out << "    QString targetNamespace;" << endl;
    out << "    " << msgReplyType << " reply;" << endl;
    out << "    QMap<QString, QVariant> parameters;" << endl;
    out << "    " << msgReplyType << " " << msgReplyName << ";" << endl;
    out << "    QNetworkAccessManager *manager;" << endl;
    out << "    QNetworkReply *networkReply;" << endl;
    out << "    QByteArray data;" << endl;
    out << "};" << endl;
    out << "#endif // Q" << msgName.toUpper() << endl;
    // EOF (SOAP message)
    // ---------------------------------

    file.close();
    return true;
}

bool StandardPath::createMessageSource(QSoapMessage *msg)
{
    QString msgName = msg->getMessageName();
    QString msgReplyName = msg->getReturnValueName().first(); // Possible problem in case of multi-return.

    QMap<QString, QVariant> tempMap = msg->getReturnValueNameType();
    QString msgReplyType = "";
    foreach (QString s, tempMap.keys())
    {
        msgReplyType += tempMap.value(s).typeName();
        break;
    }

    tempMap.clear();
    tempMap = msg->getParameterNamesTypes();
    QString msgParameters = "";
    foreach (QString s, tempMap.keys())
    {
        msgParameters += QString(tempMap.value(s).typeName()) + " " + s + ",";
    }
    msgParameters.chop(1);


    QFile file(workingDir.path() + "/" + msgName + ".cpp");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return false;

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out << "#include \"../headers/" << msgName << ".h\"" << endl;
    out << endl;
    out << msgName << "::" << msgName << "(QObject *parent) :" << endl;
    out << "    QObject(parent)" << endl;
    out << "{" << endl;
    out << "    init();" << endl;
    out << "    hostname = \"\";" << endl;
    out << "    hostUrl.setHost(hostname);" << endl;
    out << "    messageName = \"" << msgName << "\";" << endl;
    out << "    parameters.clear();" << endl;
    out << "}" << endl;
    /*
out << "
out << "QSoapMessage::QSoapMessage(QUrl url, QString _messageName, QObject *parent) :
out << "    QObject(parent), hostUrl(url), messageName(_messageName)
out << "{
out << "    init();
out << "    hostname = hostUrl.host();
out << "    parameters.clear();
out << "}
out << "
out << "QSoapMessage::QSoapMessage(QString url, QString _messageName, QObject *parent) :
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
*/
    out << endl;
    out << msgName << "::~" << msgName << "()" << endl;
    out << "{" << endl;
    out << "    delete manager;" << endl;
    out << "    delete networkReply;" << endl;
    out << "    this->deleteLater();" << endl;
    out << "}" << endl;
    out << endl;
    out << "void " << msgName << "::setParams(" << msgParameters << ")" << endl;
    out << "{" << endl;
    out << "    parameters = params;" << endl; //NEEDS FIXING TO PROPERLY ASSIGN PARAMS TO VARIABLES!
    out << "    returnValue = returnVal;" << endl;
    out << "}" << endl;
    out << endl;
    out << "void " << msgName << "::setTargetNamespace(QString tNamespace)" << endl;
    out << "{" << endl;
    out << "    targetNamespace = tNamespace;" << endl;
    out << "}" << endl;
    out << endl;
    out << "void " << msgName << "::setProtocol(Protocol prot)" << endl;
    out << "{" << endl;
    out << "    protocol = prot;" << endl;
    out << "}" << endl;
    out << endl;
    out << "bool " << msgName << "::sendMessage()" << endl;
    out << "{" << endl;
    out << "    hostUrl.setUrl(hostname);" << endl;
    out << "    QNetworkRequest request;" << endl;
    out << "    request.setUrl(hostUrl);" << endl;
    out << "    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(\"application/soap+xml; charset=utf-8\"));" << endl;
    out << "    if (protocol == soap10)" << endl;
    out << "        request.setRawHeader(QByteArray(\"SOAPAction\"), QByteArray(hostUrl.toString().toAscii()));" << endl;
    out << endl;
    out << "    prepareRequestData();" << endl;
    out << endl;
    out << "    qDebug() << request.rawHeaderList() << \" \" << request.url().toString();" << endl;
    out << "    qDebug() << \"*************************\";" << endl;
    out << endl;
    out << "    manager->post(request, data);" << endl;
    out << "    return true;" << endl;
    out << "}" << endl;
    out << endl;
    out << "bool " << msgName << "::sendMessage(" << msgParameters << ")" << endl;
    out << "{" << endl;
    out << "    parameters = params;" << endl; // FIX THAT!
    out << "    sendMessage();" << endl;
    out << "    return true;" << endl;
    out << "}" << endl;
    out << endl;
    out << "/* STATIC */" << endl;
    out << "" << msgReplyType << " " << msgName << "::sendMessage(QObject *parent, QUrl url, QString _messageName, " << msgParameters << ")" << endl;
    out << "{" << endl;
    out << "    " << msgName << " qsm(url.host(), _messageName, params, returnVal, parent);" << endl;
    out << "    qsm.role = staticRole;" << endl;
    out << "    qsm.hostUrl = url;" << endl;
    out << endl;
    out << "    qsm.sendMessage();" << endl;
    out << "    // TODO: ADD ERROR HANDLING!" << endl;
    out << "    forever" << endl;
    out << "    {" << endl;
    out << "        if (qsm.replyReceived)" << endl;
    out << "            return qsm.reply;" << endl;
    out << "        else" << endl;
    out << "        {" << endl;
    out << "            qApp->processEvents();" << endl;
    out << "        }" << endl;
    out << "    }" << endl;
    out << "}" << endl;
    out << endl;
    out << "" << msgReplyType << " " << msgName << "::replyRead()" << endl;
    out << "{" << endl;
    out << "    return reply;" << endl;
    out << "}" << endl;
    out << endl;
    out << "QString " << msgName << "::getMessageName()" << endl;
    out << "{" << endl;
    out << "    return messageName;" << endl;
    out << "}" << endl;
    out << endl;
    out << "QStringList " << msgName << "::getParameterNames() const" << endl;
    out << "{" << endl;
    out << "    return (QStringList) parameters.keys();" << endl;
    out << "}" << endl;
    out << endl;
    out << "QString " << msgName << "::getReturnValueName() const" << endl;
    out << "{" << endl;
    out << "    return \"" << msgReplyName << "\";" << endl;
    out << "}" << endl;
    out << endl;
    out << "QMap<QString, QVariant> " << msgName << "::getParameterNamesTypes() const" << endl;
    out << "{" << endl;
    out << "    return parameters;" << endl;
    out << "}" << endl;
    out << endl;
    out << "QString " << msgName << "::getReturnValueNameType() const" << endl;
    out << "{" << endl;
    out << "    return returnValue;" << endl;
    out << "}" << endl;
    out << endl;
    out << "QString " << msgName << "::getTargetNamespace()" << endl;
    out << "{" << endl;
    out << "    return targetNamespace;" << endl;
    out << "}" << endl;
    out << endl;
    out << "void " << msgName << "::replyFinished(QNetworkReply *netReply)" << endl;
    out << "{" << endl;
    out << "    networkReply = netReply;" << endl;
    out << "    QByteArray replyBytes;" << endl;
    out << endl;
    out << "    replyBytes = (networkReply->readAll());" << endl;
    out << "    QString replyString = convertReplyToUtf(replyBytes);" << endl;
    out << endl;
    out << "    QString tempBegin = \"<\" + messageName + \"Result>\";" << endl;
    out << "    int replyBeginIndex = replyString.indexOf(tempBegin, 0, Qt::CaseSensitive);" << endl;
    out << "    replyBeginIndex += tempBegin.length();" << endl;
    out << endl;
    out << "    QString tempFinish = \"</\" + messageName + \"Result>\";" << endl;
    out << "    int replyFinishIndex = replyString.indexOf(tempFinish, replyBeginIndex, Qt::CaseSensitive);" << endl;
    out << endl;
    out << "    if (replyBeginIndex == -1)" << endl;
    out << "        replyBytes = 0;" << endl;
    out << "    if (replyFinishIndex == -1)" << endl;
    out << "        replyFinishIndex = replyString.length();" << endl;
    out << endl;
    out << "    reply = (QVariant) replyString.mid(replyBeginIndex, replyFinishIndex - replyBeginIndex);" << endl;
    out << endl;
    out << "    replyReceived = true;" << endl;
    out << "    emit replyReady(reply);" << endl;
    out << "}" << endl;
    out << endl;
    out << "void QSoapMessage::init()" << endl;
    out << "{" << endl;
    out << "    protocol = soap12;" << endl;
    out << "    replyReceived = false;" << endl;
    out << endl;
    out << "    manager = new QNetworkAccessManager(this);" << endl;
    out << endl;
    out << "    reply.clear();" << endl;
    out << "    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));" << endl;
    out << "}" << endl;
    out << endl;
    out << "void " << msgName << "::prepareRequestData()" << endl;
    out << "{" << endl;
    out << "    data.clear();" << endl;
    out << "    QString header, body, footer;" << endl;
    out << endl;
    out << "    if (protocol == soap12)" << endl;
    out << "    {" << endl;
    out << "        header = \"<?xml version=\\\"1.0\\\" encoding=\\\"utf-8\\\"?> \\r\\n <soap12:Envelope xmlns:xsi=\\\"http://www.w3.org/2001/XMLSchema-instance\\\" xmlns:xsd=\\\"http://www.w3.org/2001/XMLSchema\\\" xmlns:soap12=\\\"http://www.w3.org/2003/05/soap-envelope\\\"> \\r\\n <soap12:Body> \\r\\n\";" << endl;
    out << endl;
    out << "        footer = \"</soap12:Body> \\r\\n</soap12:Envelope>\";" << endl;
    out << "    }" << endl;
    out << endl;
    out << "    body = \"\\t<\" + messageName + \" xmlns=\"\" + targetNamespace + \"\"> \\r\\n\";" << endl;
    out << endl;
    out << "    foreach (const QString currentKey, parameters.keys())" << endl;
    out << "    {" << endl;
    out << "        QVariant qv = parameters.value(currentKey);" << endl;
    out << "        // Currently, this does not handle nested lists" << endl;
    out << "        body += \"\\t\\t<\" + currentKey + \">\" + qv.toString() + \"</\" + currentKey + \"> \\r\\n\";" << endl;
    out << "    }" << endl;
    out << endl;
    out << "    body += \"\\t</\" + messageName + \"> \\r\\n\";" << endl;
    out << endl;
    out << "    data.append(header + body + footer);" << endl;
    out << "}" << endl;
    out << endl;
    out << "QString " << msgName << "::convertReplyToUtf(QString textToConvert)" << endl;
    out << "{" << endl;
    out << "    QString result = textToConvert;" << endl;
    out << endl;
    out << "    result.replace(\"&lt;\", \"<\");" << endl;
    out << "    result.replace(\"&gt;\", \">\");" << endl;
    out << endl;
    out << "    return result;" << endl;
    out << "}" << endl;
    // EOF (SOAP message)
    // ---------------------------------

    file.close();
    return true;
}
