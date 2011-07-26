#include "../headers/standardpath.h"

/*!
    \class StandardPath
    \brief Creates code in the standard path (standard structure).
  */

/*!
    \fn StandardPath::StandardPath(QObject *parent)
  */
StandardPath::StandardPath(QObject *parent) :
    QObject(parent)
{
    errorState = false;
    errorMessage = "";
}

/*!
    \fn StandardPath::isErrorState()
  */
bool StandardPath::isErrorState()
{
    return errorState;
}

/*!
    \fn StandardPath::enterErrorState(QString errMessage)
  */
bool StandardPath::enterErrorState(QString errMessage)
{
    errorState = true;
    errorMessage += errMessage + " ";
    qDebug() << errorMessage;
    emit errorEncountered(errMessage);
    return false;
}

/*!
    \fn StandardPath::prepare()
  */
void StandardPath::prepare()
{
    workingDir.mkdir("headers");
    workingDir.mkdir("sources");

    messages = wsdl->getMethods();
}

/*!
    \fn StandardPath::create(QWsdl *w, QDir wrkDir, Flags flgs, QString bsClsNme, QObject *parent)
  */
bool StandardPath::create(QWsdl *w, QDir wrkDir, Flags flgs, QString bsClsNme, QObject *parent)
{
    StandardPath obj(parent);
    obj.baseClassName = bsClsNme;
    obj.flags = flgs;
    obj.workingDir = wrkDir;
    obj.wsdl = w;
    obj.prepare();

    if (!obj.createMessages())
        return false;
    if (!obj.createService())
        return false;
    if (!obj.createBuildSystemFile())
        return false;
    return true;
}

/*!
    \fn StandardPath::createMessages()
  */
bool StandardPath::createMessages()
{
    workingDir.cd("headers");
    foreach (QString s, messages->keys())
    {
        QSoapMessage *m = messages->value(s);
        if (!createMessageHeader(m))
            return enterErrorState("Creating header for message \"" + m->getMessageName() + "\" failed!");
    }

    workingDir.cdUp();
    workingDir.cd("sources");
    foreach (QString s, messages->keys())
    {
        QSoapMessage *n = messages->value(s);
        if (!createMessageSource(n))
            return enterErrorState("Creating source for message \"" + n->getMessageName() + "\" failed!");;
    }

    workingDir.cdUp();
    return true;
}

/*!
    \fn StandardPath::createMessageHeader(QSoapMessage *msg)
  */
bool StandardPath::createMessageHeader(QSoapMessage *msg)
{
    QString msgName = msg->getMessageName();
    QFile file(workingDir.path() + "/" + msgName + ".h");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open message header file for writing.");

    QString msgReplyName = msg->getReturnValueName().first(); // Possible problem in case of multi-return.

    QString msgReplyType = "";
    QString msgParameters = "";
    {
        // Create msgReplyType
        QMap<QString, QVariant> tempMap = msg->getReturnValueNameType();
        foreach (QString s, tempMap.keys())
        {
            msgReplyType += tempMap.value(s).typeName();
            break;
        }

        tempMap.clear();
        tempMap = msg->getParameterNamesTypes();

        // Create msgParameters (comma separated list)
        foreach (QString s, tempMap.keys())
        {
            msgParameters += QString(tempMap.value(s).typeName()) + " " + s + ", ";
        }
        msgParameters.chop(2);
    }

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "#ifndef " << msgName.toUpper() << endl; // might break on curious names
    out << "#define " << msgName.toUpper() << endl;
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
    out << "    ~" << msgName << "();" << endl;
    out << endl;
    out << "    void setParams(" << msgParameters << ");" << endl;
    if (flags.mode != Flags::compactMode)
        out << "    void setProtocol(Protocol protocol);" << endl;
    out << "    bool sendMessage();" << endl;
    out << "    bool sendMessage(" << msgParameters << ");" << endl;
    out << "    " << msgReplyType << " static sendMessage(QObject *parent, QUrl url, QString _messageName," << endl;
    out << "                                " << msgParameters << ");" << endl;
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
//    out << "    void init();" << endl;
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
    { // Create parameters list in declarative form.
        out << "    // -------------------------" << endl << "    // Parameters:" << endl;
        QMap<QString, QVariant> tempMap = msg->getParameterNamesTypes();
        foreach (QString s, tempMap.keys())
        {
            out << "    " << tempMap.value(s).typeName() << " " << s  << ";" << endl;
        }
        out << "    // End of parameters." << endl << "    // -------------------------" << endl;
    }
    out << "    " << msgReplyType << " " << msgReplyName << ";" << endl;
    out << "    QNetworkAccessManager *manager;" << endl;
    out << "    QNetworkReply *networkReply;" << endl;
    out << "    QByteArray data;" << endl;
    out << "};" << endl;
    out << "#endif // " << msgName.toUpper() << endl;
    // EOF (SOAP message)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \fn StandardPath::createMessageSource(QSoapMessage *msg)
  */
bool StandardPath::createMessageSource(QSoapMessage *msg)
{
    QString msgName = msg->getMessageName();
    QFile file(workingDir.path() + "/" + msgName + ".cpp");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open message source file for writing.");

    QString msgReplyName = msg->getReturnValueName().first(); // Possible problem in case of multi-return.

    QString msgReplyType = "";
    QString msgParameters = "";
    {
        QMap<QString, QVariant> tempMap = msg->getReturnValueNameType();
        foreach (QString s, tempMap.keys())
        {
            msgReplyType += tempMap.value(s).typeName();
            break;
        }

        tempMap.clear();
        tempMap = msg->getParameterNamesTypes();

        foreach (QString s, tempMap.keys())
        {
            msgParameters += QString(tempMap.value(s).typeName()) + " " + s + ", ";
        }
        msgParameters.chop(2);
    }

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out << "#include \"../headers/" << msgName << ".h\"" << endl;
    out << endl;
    out << msgName << "::" << msgName << "(QObject *parent) :" << endl;
    out << "    QObject(parent)" << endl;
    out << "{" << endl;
//    out << "    init();" << endl;
    out << "    hostname = \"" << msg->getTargetNamespace() << "\";" << endl;
    out << "    hostUrl.setHost(hostname);" << endl;
    out << "    messageName = \"" << msgName << "\";" << endl;
    { // Defaulting the protocol:
        out << "    protocol = ";
        if (flags.protocol == QSoapMessage::soap10)
            out << "soap10";
        else if (flags.protocol == QSoapMessage::soap12)
            out << "soap12";
        else if (flags.protocol == QSoapMessage::http)
            out << "http";
        out << ";" << endl;
    }
//    out << "    parameters.clear();" << endl;
    out << "}" << endl;
    out << msgName << "::" << msgName << "(" << msgParameters << ", QObject *parent) :" << endl;
    out << "    QObject(parent)" << endl;
    out << "{" << endl;
    { // Assign all parameters.
        QMap<QString, QVariant> tempMap = msg->getParameterNamesTypes();
        foreach (QString s, tempMap.keys())
        {
            out << "    this." << s << " = " << s << ";" << endl;
        }

      // Defaulting the protocol:
        out << "    protocol = ";
        if (flags.protocol == QSoapMessage::soap10)
            out << "soap10";
        else if (flags.protocol == QSoapMessage::soap12)
            out << "soap12";
        else if (flags.protocol == QSoapMessage::http)
            out << "http";
        out << ";" << endl;
    }
//    out << "    init();" << endl;
    out << "    hostUrl.setHost(hostname + messageName);" << endl;
    out << "}" << endl;
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
    { // Assign all parameters.
        QMap<QString, QVariant> tempMap = msg->getParameterNamesTypes();
        foreach (QString s, tempMap.keys())
        {
            out << "    this." << s << " = " << s << ";" << endl;
        }
    }
    out << "}" << endl;
    out << endl;
    if (flags.mode != Flags::compactMode)
    {
        out << "void " << msgName << "::setProtocol(Protocol prot)" << endl;
        out << "{" << endl;
        out << "    protocol = prot;" << endl;
        out << "}" << endl;
        out << endl;
    }
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
    if (flags.mode == Flags::debugMode)
    {
        out << "    qDebug() << request.rawHeaderList() << \" \" << request.url().toString();" << endl;
        out << "    qDebug() << \"*************************\";" << endl;
        out << endl;
    }
    out << "    manager->post(request, data);" << endl;
    out << "    return true;" << endl;
    out << "}" << endl;
    out << endl;
    out << "bool " << msgName << "::sendMessage(" << msgParameters << ")" << endl;
    out << "{" << endl;
//    out << "    parameters = params;" << endl; // FIX THAT!
    { // Assign all parameters.
        QMap<QString, QVariant> tempMap = msg->getParameterNamesTypes();
        foreach (QString s, tempMap.keys())
        {
            out << "    this." << s << " = " << s << ";" << endl;
        }
    }

    out << "    sendMessage();" << endl;
    out << "    return true;" << endl;
    out << "}" << endl;
    out << endl;
    out << "/* STATIC */" << endl;
    out << "" << msgReplyType << " " << msgName << "::sendMessage(QObject *parent, " << msgParameters << ")" << endl;
    out << "{" << endl;
    out << "    " << msgName << " qsm(parent);" << endl;
    { // Assign all parameters.
        out << "    qsm.setParameters(";
        QString tempS = "";
        QMap<QString, QVariant> tempMap = msg->getParameterNamesTypes();
        foreach (QString s, tempMap.keys())
        {
            tempS += s + ", ";
        }
        tempS.chop(2);
        out << tempS << ");" << endl;
    }
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
    out << "    QMap<QString, QVariant> parameters;" << endl;
    { // Assign all parameters.
        QMap<QString, QVariant> tempMap = msg->getParameterNamesTypes();
        foreach (QString s, tempMap.keys())
        {
            out << "    parameters.insert(" << s << ", QVariant(" << tempMap.value(s).typeName();
            out << "(" << tempMap.value(s).toString() << ")));" << endl;
        }
    }
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
    out << "    QMap<QString, QVariant> parameters;" << endl;
    { // Assign all parameters.
        QMap<QString, QVariant> tempMap = msg->getParameterNamesTypes();
        foreach (QString s, tempMap.keys())
        {
            out << "    parameters.insert(" << s << ", QVariant(" << tempMap.value(s).typeName();
            out << "(" << tempMap.value(s).toString() << ")));" << endl;
        }
    }
    out << "    return parameters;" << endl;
    out << "}" << endl;
    out << endl;
    out << "QString " << msgName << "::getReturnValueNameType() const" << endl;
    out << "{" << endl;
    out << "    return " << msgReplyType << ";" << endl;
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

/*!
    \fn StandardPath::createService()
  */
bool StandardPath::createService()
{
    workingDir.cd("headers");
    if (!createServiceHeader())
        return enterErrorState("Creating header for Web Service \"" + wsdl->getWebServiceName() + "\" failed!");

    workingDir.cdUp();
    workingDir.cd("sources");
    if (!createServiceSource())
        return enterErrorState("Creating source for Web Service \"" + wsdl->getWebServiceName() + "\" failed!");

    workingDir.cdUp();
    return true;
}

/*!
    \fn StandardPath::createServiceHeader()
  */
bool StandardPath::createServiceHeader()
{
    QString wsName = "";
    if (baseClassName != "")
        wsName = baseClassName;
    else
        wsName = wsdl->getWebServiceName();

    QFile file(workingDir.path() + "/" + wsName + ".h");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open Web Service header file for writing.");

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "#ifndef " << wsName.toUpper() << "_H" << endl;
    out << "#define " << wsName.toUpper() << "_H" << endl;
    out << endl;
    out << "#include <QUrl>" << endl;
    { // Include all messages.
        QStringList tempMap = wsdl->getMethodNames();
        foreach (QString s, tempMap)
        {
            out << "#include \"" << s << ".h\"" << endl;
        }
    }
    out << endl;
    out << "class " << wsName << " : public QObject" << endl;
    out << "{" << endl;
    out << "    Q_OBJECT" << endl;
    out << endl;
    out << "public:" << endl;
    out << "    " << wsName << "(QObject *parent = 0);" << endl;
    out << "    ~" << wsName << "();" << endl;
    out << endl;
    out << "    QStringList getMethodNames();" << endl;
    { // Declare all messages (as wrappers for message classes).
        QMap<QString, QSoapMessage *> *tempMap = wsdl->getMethods();
        foreach (QString s, tempMap->keys())
        {
            QString tmpS = "", tmpP = "";
            QSoapMessage *m = tempMap->value(s);
            foreach (QString ret, m->getReturnValueNameType().keys())
            {
                tmpS = m->getReturnValueNameType().value(ret).typeName();
                break; // This does not support multiple return values!
            }

            QMap<QString, QVariant> tempParam = m->getParameterNamesTypes();
            // Create msgParameters (comma separated list)
            foreach (QString param, tempParam.keys())
            {
                tmpP += QString(tempParam.value(param).typeName()) + " " + param + ", ";
            }
            tmpP.chop(2);

            out << "    " << tmpS << " ";
            out << s << "(" << tmpP << ");" << endl;
        }
    }
    out << endl;
    out << "    QUrl getHostUrl();" << endl;
    out << "    QString getHost();" << endl;
    out << "    bool isErrorState();" << endl;
    out << endl;
    out << "protected:" << endl;
    out << "    void init();" << endl;
    out << endl;
    out << "    bool errorState;" << endl;
    out << "    QUrl hostUrl;" << endl;
    out << "    QString hostname;" << endl;
    out << "};" << endl;
    out << endl;
    out << "#endif // " << wsName.toUpper() << "_H" << endl;
    // EOF (Web Service header)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \fn StandardPath::createServiceSource()
  */
bool StandardPath::createServiceSource()
{
    QString wsName = "";
    if (baseClassName != "")
        wsName = baseClassName;
    else
        wsName = wsdl->getWebServiceName();

    QFile file(workingDir.path() + "/" + wsName + ".cpp");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open Web Service source file for writing.");

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "#include \"../headers/" << wsName << ".h\"" << endl;
    out << endl;
    out << "" << wsName << "::" << wsName << "(QObject *parent)" << endl;
    out << "    : QObject(parent)" << endl;
    out << "{" << endl;
//    out << "    connect(" << endl;
    out << "    errorState = false;" << endl;
    out << "    isErrorState();" << endl;
    out << "}" << endl;
    out << endl;
    out << "" << wsName << "::~" << wsName << "()" << endl;
    out << "{" << endl;
    out << "}" << endl;
    out << endl;
    out << "QStringList " << wsName << "::getMethodNames()" << endl;
    out << "{" << endl;
    out << "    return (QStringList) messages->keys();" << endl;
    out << "}" << endl;
    out << endl;
    { // Define all messages (as wrappers for message classes).
        QMap<QString, QSoapMessage *> *tempMap = wsdl->getMethods();
        foreach (QString s, tempMap->keys())
        {
            QString tmpS = "", tmpP = "", tmpPN = "";
            QSoapMessage *m = tempMap->value(s);
            foreach (QString ret, m->getReturnValueNameType().keys())
            {
                tmpS = m->getReturnValueNameType().value(ret).typeName();
                break; // This does not support multiple return values!
            }

            QMap<QString, QVariant> tempParam = m->getParameterNamesTypes();
            // Create msgParameters (comma separated list)
            foreach (QString param, tempParam.keys())
            {
                tmpP += QString(tempParam.value(param).typeName()) + " " + param + ", ";
                tmpPN += param + ", ";
            }
            tmpP.chop(2);

            if (flags.synchronousness == Flags::synchronous)
            {
                out << tmpS << " " << wsName << "::" << s << "(" << tmpP << ");" << endl;
                out << "{" << endl;
                out << "    return " << m->getMessageName() << "::sendMessage(" << tmpPN << ");" << endl;
                out << "}" << endl;
                out << endl;
            }
            else if (flags.synchronousness == Flags::asynchronous)
            {
                QString objName = m->getMessageName().toLower(); // might crash when WS name is in low case
                out << tmpS << " " << s << "(" << tmpP << ");" << endl;
                out << "{" << endl;
                out << "    " << m->getMessageName() << " " << objName << "(this);" << endl;
                out << "    " << objName << ".sendMessage(" << tmpPN << ");" << endl;
                out << "}" << endl;
                out << endl;
            }
        }
    }
    out << "QUrl " << wsName << "::getHostUrl()" << endl;
    out << "{" << endl;
    out << "    return hostUrl;" << endl;
    out << "}" << endl;
    out << endl;
    out << "QString " << wsName << "::getHost()" << endl;
    out << "{" << endl;
    out << "    return hostname;" << endl;
    out << "}" << endl;
    out << endl;
    out << "bool " << wsName << "::isErrorState()" << endl;
    out << "{" << endl;
    out << "    return errorState;" << endl;
    out << "}" << endl;
    out << endl;
    out << "void " << wsName << "::init()" << endl;
    out << "{" << endl;
    out << "    errorState = false;" << endl;
    out << endl;
    out << "    if (isErrorState())" << endl;
    out << "        return;" << endl;
    out << "}" << endl;
    // EOF (Web Service source)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \fn StandardPath::createBuildSystemFile()
  */
bool StandardPath::createBuildSystemFile()
{
    if (flags.buildSystem == Flags::qmake)
        return createQMakeProject();
    else if (flags.buildSystem == Flags::noBuildSystem)
        return true;

    return true;
}

/*!
    \fn StandardPath::createQMakeProject()
  */
bool StandardPath::createQMakeProject()
{
    QString wsName = "";
    if (baseClassName != "")
        wsName = baseClassName;
    else
        wsName = wsdl->getWebServiceName();

    QFile file(workingDir.path() + "/" + wsName + ".pro");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open Web Service .pro file for writing.");

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "#-------------------------------------------------" << endl;
    out << "#" << endl;
    out << "# Project generated from WSDL by qtWsdlConverter ";
    out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") << endl;
//    out << "# Tomasz 'sierdzio' Siekierda" << endl;
//    out << "# sierdzio@gmail.com" << endl;
    out << "#-------------------------------------------------" << endl;
    out << endl;
    out << "QT += core network" << endl;
    out << "QT -= gui" << endl;
    out << endl;
    out << "TARGET = " << wsName << endl;
    out << endl;
    out << "TEMPLATE = app" << endl;
    out << endl;
    out << "SOURCES += " << wsName << ".cpp \\" << endl;
    { // Include all sources.
        QStringList tempMap = wsdl->getMethodNames();
        foreach (QString s, tempMap)
        {
            out << "    sources/" << s << ".cpp";
            if (tempMap.indexOf(s) != (tempMap.length() - 1))
                out << " \\" << endl;
        }
    }
    out << endl;
    out << endl;
    out << "HEADERS += " << wsName << ".h \\" << endl;
    { // Include all headers.
        QStringList tempMap = wsdl->getMethodNames();
        foreach (QString s, tempMap)
        {
            out << "    headers/" << s << ".h";
            if (tempMap.indexOf(s) != (tempMap.length() - 1))
                out << " \\" << endl;
        }
    }
    // EOF (QMake .pro file)
    // ---------------------------------

    file.close();
    return true;
}
