#include "../headers/codegenerator.h"

/*!
    \class CodeGenerator
    \brief Creates code in the standard path (standard structure).

    This class will most probably be renamed, as it turns out to do almost everything in the conversion process.
  */

/*!
    \fn CodeGenerator::CodeGenerator(QObject *parent)

    Constructs QObject using \a parent, initialises CodeGenerator.
  */
CodeGenerator::CodeGenerator(QObject *parent) :
    QObject(parent)
{
    errorState = false;
    errorMessage = "";
}

/*!
    \fn CodeGenerator::errorEncountered(QString errMessage)

    Singal emitted when CodeGenerator encounters an error. Carries \a errMessage for convenience.
  */

/*!
    \fn CodeGenerator::isErrorState()

    Returns true if object is in error state.
  */
bool CodeGenerator::isErrorState()
{
    return errorState;
}

/*!
    \internal
    \fn CodeGenerator::enterErrorState(QString errMessage)
  */
bool CodeGenerator::enterErrorState(QString errMessage)
{
    errorState = true;
    errorMessage += errMessage + " ";
    qDebug() << errorMessage;
    emit errorEncountered(errMessage);
    return false;
}

/*!
    \internal
    \fn CodeGenerator::prepare()
  */
void CodeGenerator::prepare()
{
    if (!(flags.flags() & Flags::allInOneDirStructure))
    {
        workingDir.mkdir("headers");
        workingDir.mkdir("sources");
    }

    messages = wsdl->getMethods();
}

/*!
    \fn CodeGenerator::create(QWsdl *wsdl, QDir workingDir, Flags flgs, QString baseClassName = 0, QObject *parent = 0)

    Performs the conversion in CodeGenerator. Data from WSDL (\a wsdl) is combined with options specified
    in flags (\a flgs), and base class name (\a baseClassName) to create a complete set of classes
    in given directory (\a workingDir). For Qt reasons, \a parent is also needed, although it defaults to 0.

    Returns true if successful.
  */
bool CodeGenerator::create(QWsdl *w, QDir wrkDir, Flags flgs, QString bsClsNme, QObject *parent)
{
    CodeGenerator obj(parent);
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
    \internal
    \fn CodeGenerator::createMessages()
  */
bool CodeGenerator::createMessages()
{
    if (!(flags.flags() & Flags::allInOneDirStructure))
        workingDir.cd("headers");

    foreach (QString s, messages->keys())
    {
        QSoapMessage *m = messages->value(s);
        if (!createMessageHeader(m))
            return enterErrorState("Creating header for message \"" + m->getMessageName() + "\" failed!");
    }

    if (!(flags.flags() & Flags::allInOneDirStructure))
    {
        workingDir.cdUp();
        workingDir.cd("sources");
    }
    foreach (QString s, messages->keys())
    {
        QSoapMessage *n = messages->value(s);
        if (!createMessageSource(n))
            return enterErrorState("Creating source for message \"" + n->getMessageName() + "\" failed!");;
    }
    if (!(flags.flags() & Flags::allInOneDirStructure))
    {
        createMainCpp();
        workingDir.cdUp();
    }
    return true;
}

/*!
    \internal
    \fn CodeGenerator::createMessageHeader(QSoapMessage *msg)
  */
bool CodeGenerator::createMessageHeader(QSoapMessage *msg)
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
    out << "#ifndef " << msgName.toUpper() << "_H" << endl; // might break on curious names
    out << "#define " << msgName.toUpper() << "_H" << endl;
    out << endl;
    out << "#include <QtCore>" << endl;
    out << "#include <QNetworkAccessManager>" << endl;
    out << "#include <QNetworkRequest>" << endl;
    out << "#include <QNetworkReply>" << endl;
    out << endl;
    out << "class " << msgName << " : public QObject" << endl;
    out << "{" << endl;
    out << "    Q_OBJECT" << endl;
    out << "    Q_ENUMS(Protocol)" << endl;
    out << "    Q_FLAGS(Protocols)" << endl;
    out << endl;
    out << "public:" << endl;
    out << "    enum Protocol" << endl << "    {" << endl;
    out << "        http   = 0x01," << endl;
    out << "        soap10 = 0x02," << endl;
    out << "        soap12 = 0x04," << endl;
    out << "        soap   = 0x06," << endl;
    out << "        json   = 0x08" << endl;
    out << "    };" << endl;
    out << "    Q_DECLARE_FLAGS(Protocols, Protocol)" << endl;
    out << endl;
    out << "    explicit " << msgName << "(QObject *parent = 0);" << endl;
    if (msgParameters != "")
        out << "    " << msgName << "(" << msgParameters << ", QObject *parent = 0);" << endl;
    out << "    ~" << msgName << "();" << endl;
    out << endl;
    out << "    void setParams(" << msgParameters << ");" << endl;
    if (!(flags.flags() & Flags::compactMode))
        out << "    void setProtocol(Protocol protocol);" << endl;
    out << "    bool sendMessage();" << endl;
    if (msgParameters != "")
        out << "    bool sendMessage(" << msgParameters << ");" << endl;
    // Temporarily, all messages will return QString!
//    out << "    " << msgReplyType << " static sendMessage(QObject *parent";
    out << "    QString static sendMessage(QObject *parent";
    if (msgParameters != "")
    {
        out << "," << endl;
        out << "                                " << msgParameters << ");" << endl;
    }
    else
        out << ");" << endl;
    // Temporarily, all messages will return QString!
//    out << "    " << msgReplyType << " replyRead();" << endl;
    out << "    QString replyRead();" << endl;
    out << "    QString getMessageName();" << endl;
    out << "    QStringList getParameterNames() const;" << endl;
    out << "    QString getReturnValueName() const;" << endl;
    out << "    QMap<QString, QVariant> getParameterNamesTypes() const;" << endl;
    out << "    QString getReturnValueNameType() const;" << endl;
    out << "    QString getTargetNamespace();" << endl;
    out << endl;
    out << "signals:" << endl;
    // Temporarily, all messages will return QString!
//    out << "    void replyReady(" << msgReplyType << " " << msgReplyName << ");" << endl;
    out << "    void replyReady(QString " << msgReplyName << ");" << endl;
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
    out << "    Protocol protocol;" << endl;
    out << "    QUrl hostUrl;" << endl;
    out << "    QString host;" << endl;
    out << "    QString messageName;" << endl;
    out << "    QString targetNamespace;" << endl;
    // Temporarily, all messages will return QString!
//    out << "    " << msgReplyType << " reply;" << endl;
    out << "    QString reply;" << endl;
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
    out << "};" << endl << endl;
    out << "Q_DECLARE_OPERATORS_FOR_FLAGS(QSoapMessage::Protocols)" << endl;
    out << "#endif // " << msgName.toUpper() << "_H" << endl;
    // EOF (SOAP message)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal
    \fn CodeGenerator::createMessageSource(QSoapMessage *msg)
  */
bool CodeGenerator::createMessageSource(QSoapMessage *msg)
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
    out << "#include \"";
    if (!(flags.flags() & Flags::allInOneDirStructure))
        out << "../headers/";
    out << msgName << ".h\"" << endl;
    out << endl;
    out << msgName << "::" << msgName << "(QObject *parent) :" << endl;
    out << "    QObject(parent)" << endl;
    out << "{" << endl;
//    out << "    init();" << endl;
    out << "    host = \"" << msg->getTargetNamespace() << "\";" << endl;
    out << "    hostUrl.setHost(host);" << endl;
    out << "    messageName = \"" << msgName << "\";" << endl;
    { // Defaulting the protocol:
        out << "    protocol = ";
        if (flags.flags() & Flags::soap10)
            out << "soap10";
        else if (flags.flags() & Flags::soap12)
            out << "soap12";
        else if (flags.flags() & Flags::http)
            out << "http";
        else if (flags.flags() & Flags::json)
            out << "json";
        out << ";" << endl;
    }
//    out << "    parameters.clear();" << endl;
    out << "}" << endl;
    out << endl;
    if (msgParameters != "")
    {
        out << msgName << "::" << msgName << "(" << msgParameters << ", QObject *parent) :" << endl;
        out << "    QObject(parent)" << endl;
        out << "{" << endl;
        { // Assign all parameters.
            QMap<QString, QVariant> tempMap = msg->getParameterNamesTypes();
            foreach (QString s, tempMap.keys())
            {
                out << "    this->" << s << " = " << s << ";" << endl;
            }

            // Defaulting the protocol:
            out << "    protocol = ";
            if (flags.flags() & Flags::soap12) // also includes 'soap'!
                out << "soap12";
            else if (flags.flags() & Flags::soap10)
                out << "soap10";
            else if (flags.flags() & Flags::http)
                out << "http";
            else if (flags.flags() & Flags::json)
                out << "json";
            out << ";" << endl;
        }
        //    out << "    init();" << endl;
        out << "    hostUrl.setHost(host + messageName);" << endl;
        out << "}" << endl;
        out << endl;
    }
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
            out << "    this->" << s << " = " << s << ";" << endl;
        }
    }
    out << "}" << endl;
    out << endl;
    if (!(flags.flags() & Flags::compactMode))
    {
        out << "void " << msgName << "::setProtocol(Protocol prot)" << endl;
        out << "{" << endl;
        out << "    if (prot == soap)" << endl;
        out << "        protocol = soap12;" << endl;
        out << "    else" << endl;
        out << "        protocol = prot;" << endl;
        out << "}" << endl;
        out << endl;
    }
    out << "bool " << msgName << "::sendMessage()" << endl;
    out << "{" << endl;
    out << "    hostUrl.setUrl(host);" << endl;
    out << "    QNetworkRequest request;" << endl;
    out << "    request.setUrl(hostUrl);" << endl;
    out << "    if (protocol & soap)" << endl;
    out << "        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(\"application/soap+xml; charset=utf-8\"));" << endl;
    out << "    else if (protocol == json)" << endl;
    out << "        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(\"application/json; charset=utf-8\"));" << endl;
    out << "    else if (protocol == http)" << endl;
    out << "        request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(\"Content-Type: application/x-www-form-urlencoded\"));" << endl;
    out << endl;
    out << "    if (protocol == soap10)" << endl;
    out << "        request.setRawHeader(QByteArray(\"SOAPAction\"), QByteArray(hostUrl.toString().toAscii()));" << endl;
    out << endl;
    out << "    prepareRequestData();" << endl;
    out << endl;
    if (flags.flags() & Flags::debugMode)
    {
        out << "    qDebug() << request.rawHeaderList() << \" \" << request.url().toString();" << endl;
        out << "    qDebug() << \"*************************\";" << endl;
        out << endl;
    }
    out << "    manager->post(request, data);" << endl;
    out << "    return true;" << endl;
    out << "}" << endl;
    out << endl;
    if (msgParameters != "")
    {
        out << "bool " << msgName << "::sendMessage(" << msgParameters << ")" << endl;
        out << "{" << endl;
        { // Assign all parameters.
            QMap<QString, QVariant> tempMap = msg->getParameterNamesTypes();
            foreach (QString s, tempMap.keys())
            {
                out << "    this->" << s << " = " << s << ";" << endl;
            }
        }
        out << "    sendMessage();" << endl;
        out << "    return true;" << endl;
        out << "}" << endl;
        out << endl;
    }
    out << "/* STATIC */" << endl;
    // Temporarily, all messages will return QString!
//    out << "" << msgReplyType << " " << msgName << "::sendMessage(QObject *parent, " << msgParameters << ")" << endl;
    out << "QString " << msgName << "::sendMessage(QObject *parent";
    if (msgParameters != "")
        out << ", " << msgParameters;
    out << ")" << endl;
    out << "{" << endl;
    out << "    " << msgName << " qsm(parent);" << endl;
    { // Assign all parameters.
        out << "    qsm.setParams(";
        QString tempS = "";
        QMap<QString, QVariant> tempMap = msg->getParameterNamesTypes();
        foreach (QString s, tempMap.keys())
        {
            tempS += s + ", ";
        }
        tempS.chop(2);
        out << tempS << ");" << endl;
    }
//    out << "    qsm.hostUrl = url;" << endl;
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
    // Temporarily, all messages will return QString!
//    out << "" << msgReplyType << " " << msgName << "::replyRead()" << endl;
    out << "QString " << msgName << "::replyRead()" << endl;
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
            out << "    parameters.insert(\"" << s << "\", QVariant(";
            QString tmpName = tempMap.value(s).typeName();
            if (tmpName != "int")
                out << tmpName;
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
            out << "    parameters.insert(\"" << s << "\", QVariant(";
            QString tmpName = tempMap.value(s).typeName();
            if (tmpName != "int")
                out << tmpName;
            out << "(" << tempMap.value(s).toString() << ")));" << endl;
        }
    }
    out << "    return parameters;" << endl;
    out << "}" << endl;
    out << endl;
    out << "QString " << msgName << "::getReturnValueNameType() const" << endl;
    out << "{" << endl;
    out << "    return \"" << msgReplyType << "\";" << endl;
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
    // Temporarily, all messages will return QString!
    out << "    reply = (QString) replyString.mid(replyBeginIndex, replyFinishIndex - replyBeginIndex);" << endl;
    out << endl;
    out << "    replyReceived = true;" << endl;
    out << "    emit replyReady(reply);" << endl;
    out << "}" << endl;
    out << endl;
    /*
    out << "void " << msgName << "::init()" << endl;
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
    */
    out << "void " << msgName << "::prepareRequestData()" << endl;
    out << "{" << endl;
    out << "    data.clear();" << endl;
    out << "    QString header, body, footer;" << endl;
    out << "QString endl = \"\\r\\n\"; // Replace with something OS-independent, or seriously rethink." << endl;
    out << "" << endl;
    out << "if (protocol & soap)" << endl;
    out << "{" << endl;
    out << "    if (protocol == soap12)" << endl;
    out << "    {" << endl;
    out << "        header = \"<?xml version=\\\"1.0\\\" encoding=\\\"utf-8\\\"?> \" + endl +" << endl;
    out << "                 \"<soap12:Envelope xmlns:xsi=\\\"http://www.w3.org/2001/XMLSchema-instance\\\" \" +" << endl;
    out << "                 \"xmlns:xsd=\\\"http://www.w3.org/2001/XMLSchema\\\" \" +" << endl;
    out << "                 \"xmlns:soap12=\\\"http://www.w3.org/2003/05/soap-envelope\\\"> \" + endl +" << endl;
    out << "                 \"<soap12:Body> \" + endl;" << endl;
    out << endl;
    out << "        footer = \"</soap12:Body> \" + endl + \"</soap12:Envelope>\";" << endl;
    out << "    }" << endl;
    out << "    else if (protocol == soap10)" << endl;
    out << "    {" << endl;
    out << "        header = \"<?xml version=\\\"1.0\\\" encoding=\\\"utf-8\\\"?> \" + endl +" << endl;
    out << "                \"<soap:Envelope xmlns:xsi=\\\"http://www.w3.org/2001/XMLSchema-instance\\\" \" +" << endl;
    out << "                \"xmlns:xsd=\\\"http://www.w3.org/2001/XMLSchema\\\" \" +" << endl;
    out << "                \"xmlns:soap=\\\"http://www.w3.org/2003/05/soap-envelope\\\"> \" + endl +" << endl;
    out << "                \"<soap:Body> \" + endl;" << endl;
    out << endl;
    out << "        footer = \"</soap:Body> \" + endl + \"</soap:Envelope>\";" << endl;
    out << "    }" << endl;
    out << endl;
    out << "    body = \"\\t<\" + messageName + \" xmlns=\"\" + targetNamespace + \"\"> \" + endl;" << endl;
    out << endl;
    out << "    foreach (const QString currentKey, parameters.keys())" << endl;
    out << "    {" << endl;
    out << "        QVariant qv = parameters.value(currentKey);" << endl;
    out << "        // Currently, this does not handle nested lists" << endl;
    out << "        body += \"\\t\\t<\" + currentKey + \">\" + qv.toString() + \"</\" + currentKey + \"> \" + endl;" << endl;
    out << "    }" << endl;
    out << endl;
    out << "    body += \"\\t</\" + messageName + \"> \" + endl;" << endl;
    out << "}" << endl;
    out << "else if (protocol == http)" << endl;
    out << "{" << endl;
    out << "    foreach (const QString currentKey, parameters.keys())" << endl;
    out << "    {" << endl;
    out << "        QVariant qv = parameters.value(currentKey);" << endl;
    out << "        // Currently, this does not handle nested lists" << endl;
    out << "        body += currentKey + \"=\" + qv.toString() + \"&\";" << endl;
    out << "    }" << endl;
    out << "    body.chop(1);" << endl;
    out << "}" << endl;
    out << "else if (protocol == json)" << endl;
    out << "{" << endl;
    out << "    body += \"{\" + endl;" << endl;
    out << "    foreach (const QString currentKey, parameters.keys())" << endl;
    out << "    {" << endl;
    out << "        QVariant qv = parameters.value(currentKey);" << endl;
    out << "        // Currently, this does not handle nested lists" << endl;
    out << "        body += \"{\" + endl + \"\\t\\\"\" + currentKey + \"\\\" : \\\"\" + qv.toString() + \"\\\"\" + endl;"
        << endl;
    out << "    }" << endl;
    out << "    body += \"}\";" << endl;
    out << "}" << endl;
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
  \internal

  Creates a dummy main.cpp file. It's needed only for successful compilation of
  freshly generated code. It is NOT NEEDED for any other reason. You can safely delete
  it fo your project.
  */
bool CodeGenerator::createMainCpp()
{
    QFile file(workingDir.path() + "/main.cpp");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open Web Service header file for writing.");

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "/*Creates a dummy main.cpp file. It's needed only for successful compilation of freshly generated code. It is NOT NEEDED for any other reason. You can safely delete it fo your project (just remember to remove it from .pro file, too). */" << endl;
    out << "#include \"../headers/band_ws.h\"" << endl;
    out << "int main() {return 0;}" << endl;
    // EOF (main.cpp)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal
    \fn CodeGenerator::createService()
  */
bool CodeGenerator::createService()
{
    if (!(flags.flags() & Flags::allInOneDirStructure))
        workingDir.cd("headers");
    if (!createServiceHeader())
        return enterErrorState("Creating header for Web Service \"" + wsdl->getWebServiceName() + "\" failed!");

    if (!(flags.flags() & Flags::allInOneDirStructure))
    {
        workingDir.cdUp();
        workingDir.cd("sources");
    }
    if (!createServiceSource())
        return enterErrorState("Creating source for Web Service \"" + wsdl->getWebServiceName() + "\" failed!");

    if (!(flags.flags() & Flags::allInOneDirStructure))
        workingDir.cdUp();
    return true;
}

/*!
    \internal
    \fn CodeGenerator::createServiceHeader()
  */
bool CodeGenerator::createServiceHeader()
{
    QString wsName = "";
    QMap<QString, QSoapMessage *> *tempMap = wsdl->getMethods();
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
        QStringList tempMp = wsdl->getMethodNames();
        foreach (QString s, tempMp)
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
        foreach (QString s, tempMap->keys())
        {
            QString tmpReturn = "", tmpP = "";
            QSoapMessage *m = tempMap->value(s);
            foreach (QString ret, m->getReturnValueNameType().keys())
            {
                tmpReturn = m->getReturnValueNameType().value(ret).typeName();
                break; // This does not support multiple return values!
            }

            QMap<QString, QVariant> tempParam = m->getParameterNamesTypes();
            // Create msgParameters (comma separated list)
            foreach (QString param, tempParam.keys())
            {
                tmpP += QString(tempParam.value(param).typeName()) + " " + param + ", ";
            }
            tmpP.chop(2);
// Temporarily, all messages will return QString!
//            out << "    " << tmpReturn << " ";
            if (flags.flags() & Flags::synchronous)
                out << "    QString ";
            else
                out << "    void ";
            out << s << "Send(" << tmpP << ");" << endl;
        }
    }
    out << endl;
    out << "    QUrl getHostUrl();" << endl;
    out << "    QString getHost();" << endl;
    out << "    bool isErrorState();" << endl;
    if (flags.flags() & Flags::asynchronous)
    { // Declare getters of methods' replies.
        out << "    // Method reply getters: " << endl;
        foreach (QString s, tempMap->keys())
        {
            if (flags.flags() & Flags::compactMode)
            {
                ; // Code compact mode here :)
            }
            else if (flags.flags() & Flags::fullMode || flags.flags() & Flags::debugMode)
            {
                QString tmpReturn = "";
                QSoapMessage *m = tempMap->value(s);
                foreach (QString ret, m->getReturnValueNameType().keys())
                {
                    tmpReturn = m->getReturnValueNameType().value(ret).typeName();
                    break; // This does not support multiple return values!
                }
                out << "    " << tmpReturn << " " << s << "ReplyRead();" << endl;
            }
        }
        out << endl;
    }
    out << endl;
    out << "protected slots:" << endl;
    if (flags.flags() & Flags::asynchronous)
    { // Declare methods for processing asynchronous replies.
        foreach (QString s, tempMap->keys())
        {
            if (flags.flags() & Flags::compactMode)
            {
                ; // Code compact mode here :)
            }
            else if (flags.flags() & Flags::fullMode || flags.flags() & Flags::debugMode)
            {
                out << "    void " << s << "Reply(QString result);" << endl;
            }
        }
        out << endl;
    }
    out << "protected:" << endl;
    out << "    void init();" << endl;
    out << endl;
    out << "    bool errorState;" << endl;
    out << "    QUrl hostUrl;" << endl;
    out << "    QString host;" << endl;
    if (flags.flags() & Flags::asynchronous
            && (flags.flags() & Flags::fullMode || flags.flags() & Flags::debugMode))
    { // Declare reply variables for asynchronous mode.
        out << "    // Message replies:" << endl;
        foreach (QString s, tempMap->keys())
        {
            QString tmpReturn = "";
            QSoapMessage *m = tempMap->value(s);
            foreach (QString ret, m->getReturnValueNameType().keys())
            {
                tmpReturn = m->getReturnValueNameType().value(ret).typeName();
                break; // This does not support multiple return values!
            }
            out << "    " << tmpReturn << " " << s << "Result;" << endl;
        }

        out << "    // Messages:" << endl;
        foreach (QString s, tempMap->keys())
        {
            out << "    " << s << " " << s.toLower() << ";" << endl;
        }
    }
    out << "};" << endl;
    out << endl;
    out << "#endif // " << wsName.toUpper() << "_H" << endl;
    // EOF (Web Service header)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal
    \fn CodeGenerator::createServiceSource()
  */
bool CodeGenerator::createServiceSource()
{
    QString wsName = "";
    QMap<QString, QSoapMessage *> *tempMap = wsdl->getMethods();
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
    out << "#include \"";
    if (!(flags.flags() & Flags::allInOneDirStructure))
        out << "../headers/";
    out << wsName << ".h\"" << endl;
    out << endl;
    out << "" << wsName << "::" << wsName << "(QObject *parent)" << endl;
    out << "    : QObject(parent)" << endl;
    out << "{" << endl;
    if (flags.flags() & Flags::asynchronous)
    { // Connect signals and slots for asynchronous mode.
        foreach (QString s, tempMap->keys())
        {
            out << "    connect(&" << s.toLower() << ", SIGNAL(replyReady(QString)), this, SLOT(";

            if (flags.flags() & Flags::compactMode)
            {
                ; // Code compact mode here :)
            }
            else if (flags.flags() & Flags::fullMode || flags.flags() & Flags::debugMode)
            {
                out << s << "Reply(QString)";
            }

            out << "));" << endl;
        }
    }
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
    { // Create and return the QStringList containing method names:
        out << "    QStringList result;" << endl;
        foreach (QString s, tempMap->keys())
        {
            QSoapMessage *m = tempMap->value(s);
            out << "    result.append(\"" << m->getMessageName() << "\");" << endl;
        }
        out << "    return result;" << endl;
    }
    out << "}" << endl;
    out << endl;
    { // Define all messages (as wrappers for message classes).
        foreach (QString s, tempMap->keys())
        {
            QString tmpReturn = "", tmpP = "", tmpPN = "";
            QSoapMessage *m = tempMap->value(s);
            foreach (QString ret, m->getReturnValueNameType().keys())
            {
                tmpReturn = m->getReturnValueNameType().value(ret).typeName();
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
            tmpPN.chop(2);

            if (flags.flags() & Flags::synchronous)
            {
                // Temporarily, all messages will return QString!
//                out << tmpReturn << " " << wsName << "::" << s << "(" << tmpP << ")" << endl;
                out << "QString " << wsName << "::" << s << "Send(" << tmpP << ")" << endl;
                out << "{" << endl;
                out << "    // TODO: You can add your own data handling here, and make the whole method return" << endl;
                out << "    //       proper type." << endl;
                out << "    return " << m->getMessageName() << "::sendMessage(this";
                if (tmpPN != "")
                    out << ", " << tmpPN << ");" << endl;
                else
                    out << ");" << endl;
                out << "}" << endl;
                out << endl;
            }
            else if (flags.flags() & Flags::asynchronous)
            {
                out << "void " << wsName << "::" << s << "Send(" << tmpP << ")" << endl;
                out << "{" << endl;
                out << "    " << s.toLower() << ".sendMessage(" << tmpPN << ");" << endl;
                out << "}" << endl;
                out << endl;
            }
        }
    }

    if (flags.flags() & Flags::asynchronous)
    { // Define getters of methods' replies.
        out << "    // Method reply getters: " << endl;
        foreach (QString s, tempMap->keys())
        {
            if (flags.flags() & Flags::compactMode)
            {
                ; // Code compact mode here :)
            }
            else if (flags.flags() & Flags::fullMode || flags.flags() & Flags::debugMode)
            {
                QString tmpReturn = "";
                QSoapMessage *m = tempMap->value(s);
                foreach (QString ret, m->getReturnValueNameType().keys())
                {
                    tmpReturn = m->getReturnValueNameType().value(ret).typeName();
                    break; // This does not support multiple return values!
                }
                out << tmpReturn << " " << wsName << "::" << s << "ReplyRead()" << endl;
                out << "{" << endl;
                out << "    return " << s << "Result;" << endl;
                out << "}" << endl;
                out << endl;
            }
        }
        out << endl;
    }

    if (flags.flags() & Flags::asynchronous
            && (flags.flags() & Flags::fullMode || flags.flags() & Flags::debugMode))
    { // Define all slots for asynchronous mode.
        foreach (QString s, tempMap->keys())
        {
            /*
            if (flags.mode == Flags::compactMode)
            {
                ; // Code compact mode here :) Will probably be ust one method.
            }
            else
            */

            QString tmpReturn = "";
            QSoapMessage *m = tempMap->value(s);
            foreach (QString ret, m->getReturnValueNameType().keys())
            {
                tmpReturn = m->getReturnValueNameType().value(ret).typeName();
                break; // This does not support multiple return values!
            }
            out << "void " << wsName << "::" << s << "Reply(QString result)" << endl;
            out << "{" << endl;
            out << "    // TODO: Add your own data handling here!" << endl;
            out << "    //" << s << "Result = your_new_value;" << endl;
            out << "}" << endl;
            out << endl;
        }
    }
    out << "QUrl " << wsName << "::getHostUrl()" << endl;
    out << "{" << endl;
    out << "    return hostUrl;" << endl;
    out << "}" << endl;
    out << endl;
    out << "QString " << wsName << "::getHost()" << endl;
    out << "{" << endl;
    out << "    return host;" << endl;
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
    \internal
    \fn CodeGenerator::createBuildSystemFile()
  */
bool CodeGenerator::createBuildSystemFile()
{
    bool result = false;

    if (flags.flags() & Flags::noBuildSystem)
        return true;
    if (flags.flags() & Flags::qmake)
        result = createQMakeProject();
    if (flags.flags() & Flags::cmake)
        result = createCMakeProject();
    if (flags.flags() & Flags::scons)
        result = createSconsProject();

    return result;
}

/*!
    \internal
    \fn CodeGenerator::createQMakeProject()
  */
bool CodeGenerator::createQMakeProject()
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
    out << "SOURCES += ";
    if (!(flags.flags() & Flags::allInOneDirStructure))
        out << "sources/";
    out << wsName << ".cpp \\" << endl;
    // Create main.cpp to prevent compile errors. This file is NOT NEEDED in any other sense.
    out << "    ";
    if (!(flags.flags() & Flags::allInOneDirStructure))
        out << "sources/";
    out << "main.cpp \\" << endl;
    { // Include all sources.
        QStringList tempMap = wsdl->getMethodNames();
        foreach (QString s, tempMap)
        {
            out << "    ";
            if (!(flags.flags() & Flags::allInOneDirStructure))
                out << "sources/";
            out << s << ".cpp";
            if (tempMap.indexOf(s) != (tempMap.length() - 1))
                out << " \\" << endl;
        }
    }
    out << endl;
    out << endl;
    out << "HEADERS += ";
    if (!(flags.flags() & Flags::allInOneDirStructure))
        out << "headers/";
    out << wsName << ".h \\" << endl;
    { // Include all headers.
        QStringList tempMap = wsdl->getMethodNames();
        foreach (QString s, tempMap)
        {
            out << "    ";
            if (!(flags.flags() & Flags::allInOneDirStructure))
                out << "headers/";
            out << s << ".h";
            if (tempMap.indexOf(s) != (tempMap.length() - 1))
                out << " \\" << endl;
        }
    }
    // EOF (QMake .pro file)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal
    \fn CodeGenerator::createCMakeProject()
  */
bool CodeGenerator::createCMakeProject()
{
    QString wsName = "";
    if (baseClassName != "")
        wsName = baseClassName;
    else
        wsName = wsdl->getWebServiceName();

    QFile file(workingDir.path() + "/CMakeLists.txt");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open Web Service .pro file for writing.");

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "project(" << wsName << ")" << endl;
    out << "cmake_minimum_required(VERSION 2.4.0)" << endl;
    out << "find_package(Qt4 REQUIRED)" << endl;
    out << "include(${QT_USE_FILE})" << endl;
    // --------------------
    // Include sources:
    out << "set(" << wsName << "_SRCS" << endl;
    out << "    ";
    if (!(flags.flags() & Flags::allInOneDirStructure))
        out << "sources/";
    out << wsName << ".cpp" << endl;
    // Create main.cpp to prevent compile errors. This file is NOT NEEDED in any other sense.
    out << "    ";
    if (!(flags.flags() & Flags::allInOneDirStructure))
        out << "sources/";
    out << "main.cpp" << endl;
    { // Include all sources.
        QStringList tempMap = wsdl->getMethodNames();
        foreach (QString s, tempMap)
        {
            out << "    ";
            if (!(flags.flags() & Flags::allInOneDirStructure))
                out << "sources/";
            out << s << ".cpp" << endl;
        }
    }
    out << ")" << endl;
    // --------------------
    // Include MOC:
    out << "set(" << wsName << "_MOC_SRCS" << endl;
    out << "    ";
    if (!(flags.flags() & Flags::allInOneDirStructure))
        out << "headers/";
    out << wsName << ".h" << endl;
    { // Include all MOC headers.
        QStringList tempMap = wsdl->getMethodNames();
        foreach (QString s, tempMap)
        {
            out << "    ";
            if (!(flags.flags() & Flags::allInOneDirStructure))
                out << "headers/";
            out << s << ".h" << endl;
        }
    }
    out << ")" << endl;
    // Add compilation instructions:
    out << "qt4_wrap_cpp(" << wsName << "_MOCS ${" << wsName << "_MOC_SRCS})" << endl;
    out << "add_definitions(-DQT_NO_DEBUG)" << endl;
    out << "add_executable(" << wsName << endl;
    out << "    ${" << wsName << "_SRCS}" << endl;
    out << "    ${" << wsName << "_MOCS})" << endl;
    out << "target_link_libraries(" << wsName << " ${QT_LIBRARIES}" << endl;
    // EOF (CMake CMakeLists.txt file)
    // ---------------------------------

    file.close();

    return true;
}

/*!
    \internal
    \fn CodeGenerator::createSconsProject()
  */
bool CodeGenerator::createSconsProject()
{
    QString wsName = "";
    if (baseClassName != "")
        wsName = baseClassName;
    else
        wsName = wsdl->getWebServiceName();

    QFile file(workingDir.path() + "/SConstruct");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open Web Service .pro file for writing.");

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "#!/usr/bin/env python" << endl;
    out << endl;
    out << "import os" << endl;
    out << endl;
    out << "QT4_PY_PATH = \".\" # Probably not needed!" << endl;
    out << "QTDIR = \".\" # WARNING! Set QTDIR properly!" << endl; // WARNING! QTDIR has to be set on end machine!
    out << endl;
    out << "pkgpath = os.environ.get(\"PKG_CONFIG_PATH\", \"\")" << endl;
    out << "pkgpath += \":%s/lib/pkgconfig\" % QTDIR" << endl;
    out << "os.environ[\"PKG_CONFIG_PATH\"] = pkgpath" << endl;
    out << "os.environ[\"QTDIR\"] = QTDIR # WARNING! As above." << endl;
    out << endl;
    out << "env = Environment(tools=[\"default\", \"qt4\"], toolpath=[QT4_PY_PATH])" << endl;
    out << "env[\"CXXFILESUFFIX\"] = \".cpp\"" << endl;
    out << endl;
    out << "env.EnableQt4Modules([\"QtCore\", \"QtNetwork\"])" << endl;
    // --------------------
    // Include sources:
    out << "sources = [" << endl;
    out << "    ";
    if (!(flags.flags() & Flags::allInOneDirStructure))
        out << "\"sources/";
    out << wsName << ".cpp\"," << endl;
    // Create main.cpp to prevent compile errors. This file is NOT NEEDED in any other sense.
    out << "    ";
    if (!(flags.flags() & Flags::allInOneDirStructure))
        out << "\"sources/";
    out << "main.cpp\"," << endl;
    { // Include all sources.
        QStringList tempMap = wsdl->getMethodNames();
        foreach (QString s, tempMap)
        {
            out << "    ";
            if (!(flags.flags() & Flags::allInOneDirStructure))
                out << "\"sources/";
            out << s << ".cpp\"";
            if (tempMap.indexOf(s) != (tempMap.length() - 1))
                out << "," << endl;
            else
                out << "]" << endl;
        }
    }
    out << ")" << endl;

    out << "env.Program(target=\"" << wsName << "\", source=[sources]" << endl;
    // EOF (SCons SConstruct file)
    // ---------------------------------

    file.close();

    return true;
}
