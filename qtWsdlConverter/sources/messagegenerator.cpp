#include "../headers/messagegenerator.h"

/*!
    \class MessageGenerator
    \brief Creates code for messages.

    Creates messages (both headers and sources) by generating the entire code
    or subclassing QWebServiceMethod (depending on whether --subclass flag is set).
  */

/*!
    \fn MessageGenerator::MessageGenerator(QMap<QString, QWebServiceMethod *> *msgs, QDir wrkDir, Flags *flgs, QObject *parent)

    Constructs QObject using \a parent, initialises MessageGenerator
    with messages (\a msgs), working directory (\a wrkDir), and flags (\a flgs).
  */
MessageGenerator::MessageGenerator(QMap<QString, QWebServiceMethod *> *msgs,
                                   QDir wrkDir, Flags *flgs, QObject *parent) :
    QObject(parent), messages(msgs), workingDir(wrkDir), flags(flgs)
{
    errorState = false;
    m_errorMessage = "";
}

/*!
  \fn MessageGenerator::errorMessage()

  Returns QString containing error message (or "" if there was no error).
  */
QString MessageGenerator::errorMessage()
{
    return m_errorMessage;
}

/*!
    \internal
    \fn MessageGenerator::enterErrorState(QString errMessage)
  */
bool MessageGenerator::enterErrorState(QString errMessage)
{
    m_errorMessage += errMessage + "\n";
    return false;
}

/*!
    \fn MessageGenerator::createMessages()

    Creates messages (both headers and sources) by generating the entire code
    or subclassing QWebServiceMethod (depending on whether --subclass flag is set).

    Returns true if successful.
  */
bool MessageGenerator::createMessages()
{
    if (flags->flags() & Flags::noMessagesStructure) {
        if (!(flags->flags() & Flags::allInOneDirStructure)) {
            workingDir.cd("sources");
            createMainCpp();
            workingDir.cdUp();
        }
        else {
            createMainCpp();
        }

        return true;
    }

    if (!(flags->flags() & Flags::allInOneDirStructure))
        workingDir.cd("headers");

    foreach (QString s, messages->keys()) {
        QWebServiceMethod *m = messages->value(s);

        if (flags->flags() & Flags::subclass) {
            if (!createSubclassedMessageHeader(m))
                return enterErrorState("Creating header for message \""
                                       + m->methodName() + "\" failed!");
        }
        else {
            if (!createMessageHeader(m))
                return enterErrorState("Creating header for message \""
                                       + m->methodName() + "\" failed!");
        }
    }

    if (!(flags->flags() & Flags::allInOneDirStructure)) {
        workingDir.cdUp();
        workingDir.cd("sources");
    }

    foreach (QString s, messages->keys()) {
        QWebServiceMethod *n = messages->value(s);

        if (flags->flags() & Flags::subclass) {
            if (!createSubclassedMessageSource(n))
                return enterErrorState("Creating header for message \""
                                       + n->methodName() + "\" failed!");
        }
        else {
            if (!createMessageSource(n))
                return enterErrorState("Creating source for message \""
                                       + n->methodName() + "\" failed!");
        }
    }

    if (!(flags->flags() & Flags::allInOneDirStructure)) {
        createMainCpp();
        workingDir.cdUp();
    }

    return true;
}

/*!
    \internal
    \fn MessageGenerator::createSubclassedMessageHeader(QWebServiceMethod *msg)
  */
bool MessageGenerator::createSubclassedMessageHeader(QWebServiceMethod *msg)
{
    QString msgName = msg->methodName();
    QFile file(workingDir.path() + "/" + msgName + ".h");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open message header file for writing.");

    // Possible problem in case of multi-return.
    QString msgReplyName = msg->returnValueName().first();

    QString msgReplyType = "";
    QString msgParameters = "";
    {
        // Create msgReplyType
        QMap<QString, QVariant> tempMap = msg->returnValueNameType();

        foreach (QString s, tempMap.keys()) {
            msgReplyType += tempMap.value(s).typeName();
            break;
        }

        tempMap.clear();
        tempMap = msg->parameterNamesTypes();

        // Create msgParameters (comma separated list)
        foreach (QString s, tempMap.keys()) {
            msgParameters += QString(tempMap.value(s).typeName())
                    + " " + s + ", ";
        }
        msgParameters.chop(2);
    }

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    // Might break on curious names
    out << "#ifndef " << msgName.toUpper() << "_H" << endl;
    out << "#define " << msgName.toUpper() << "_H" << endl;
    out << endl;
    // Should be <QWebService/QWebServiceMethod.h>
    out << "#include <QWebService>" << endl;
    out << endl;
    out << "class " << msgName << " : public QWebServiceMethod" << endl;
    out << "{" << endl;
    out << "    Q_OBJECT" << endl;
    out << endl;
    out << "public:" << endl;

    out << "    " << msgName << "(QObject *parent = 0);" << endl;
    if (msgParameters != "")
    out << "    " << msgName << "(" << msgParameters
        << ", QObject *parent = 0);" << endl;

    out << endl;
    out << "    void setParameters(" << msgParameters << ");" << endl;
    out << endl;

    out << "    using QWebServiceMethod::sendMessage;" << endl;
    if ((msgParameters != "") && !((flags->flags() & Flags::compactMode)
                                   && (flags->flags() & Flags::synchronous)))
        out << "    bool sendMessage(" << msgParameters << ");" << endl;

    if (!((flags->flags() & Flags::compactMode)
          && (flags->flags() & Flags::asynchronous)))
    {
        out << "    QString static sendMessage(QObject *parent";
        if (msgParameters != "") {
            out << ", " << msgParameters << ");" << endl;
        }
        else {
            out << ");" << endl;
        }
    }

    out << "private:" << endl;
    out << "    void configure();" << endl;
    { // Create parameters list in declarative form.
        out << "    // -------------------------" << endl
            << "    // Parameters:" << endl;

        QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
            out << "    " << tempMap.value(s).typeName() << " " << s << ";" << endl;
        }
        out << "    // End of parameters." << endl
            << "    // -------------------------" << endl;
    }
    out << "};" << endl;
    out << endl;
    out << "#endif // " << msgName.toUpper() << "_H" << endl;
    // EOF (SOAP message)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal
    \fn MessageGenerator::createSubclassedMessageSource(QWebServiceMethod *msg)
  */
bool MessageGenerator::createSubclassedMessageSource(QWebServiceMethod *msg)
{
    QString msgName = msg->methodName();
    QFile file(workingDir.path() + "/" + msgName + ".cpp");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open message source file for writing.");

    // Possible problem in case of multi-return.
    QString msgReplyName = msg->returnValueName().first();

    QString msgReplyType = "";
    QString msgParameters = "";
    {
        QMap<QString, QVariant> tempMap = msg->returnValueNameType();

        foreach (QString s, tempMap.keys()) {
            msgReplyType += tempMap.value(s).typeName();
            break;
        }

        tempMap.clear();
        tempMap = msg->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
            msgParameters += QString(tempMap.value(s).typeName())
                    + " " + s + ", ";
        }
        msgParameters.chop(2);
    }

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "#include \"";
    if (!(flags->flags() & Flags::allInOneDirStructure))
        out << "../headers/";
    out << msgName << ".h\"" << endl;
    out << endl;

    out << msgName << "::" << msgName << "(QObject *parent) : " << endl;
    out << "    QWebServiceMethod(parent)" << endl;
    out << "{" << endl;
    out << "    configure();" << endl;
    out << "}" << endl;
    out << endl;
    if (msgParameters != "") {
        out << msgName << "::" << msgName << "(" << msgParameters
            << ", QObject *parent) :" << endl;
        out << "    QWebServiceMethod(parent)" << endl;
        out << "{" << endl;

        assignAllParameters(msg, out);
        out << "    configure();" << endl;

        out << "}" << endl;
        out << endl;
    }
    out << "void " << msgName << "::setParameters(" << msgParameters << ")" << endl;
    out << "{" << endl;


    assignAllParameters(msg, out);

    out << "}" << endl;
    out << endl;

    if ((msgParameters != "") && !((flags->flags() & Flags::compactMode)
                                   && (flags->flags() & Flags::synchronous))) {
        out << "bool " << msgName << "::sendMessage(" << msgParameters << ")" << endl;
        out << "{" << endl;

        assignAllParameters(msg, out);

        out << "    sendMessage();" << endl;
        out << "    return true;" << endl;
        out << "}" << endl;
        out << endl;
    }
    if (!((flags->flags() & Flags::compactMode)
          && (flags->flags() & Flags::asynchronous))) {
        out << "/* STATIC */" << endl;
        out << "QString " << msgName << "::sendMessage(QObject *parent";
        if (msgParameters != "")
            out << ", " << msgParameters;
        out << ")" << endl;
        out << "{" << endl;
        out << "    " << msgName << " qsm(";
        { // Assign all parameters.
//            out << "    qsm.setParameters(";
            QString tempS = "";
            QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();

            foreach (QString s, tempMap.keys()) {
                tempS += s + ", ";
            }
//            tempS.chop(2);
            out << tempS << "parent);" << endl;
        }
        out << endl;
        out << "    qsm.sendMessage();" << endl;
        out << "    // TODO: ADD ERROR HANDLING!" << endl;
        out << "    forever" << endl;
        out << "    {" << endl;
        out << "        if (qsm.replyReceived)" << endl;
        out << "            return qsm.reply.toString();" << endl;
        out << "        else" << endl;
        out << "        {" << endl;
        out << "            qApp->processEvents();" << endl;
        out << "        }" << endl;
        out << "    }" << endl;
        out << "}" << endl;
        out << endl;
    }
    out << "void " << msgName << "::" << "configure()" << endl;
    out << "{" << endl;
    out << "    m_hostUrl = QUrl(\"" << msg->host() << "\");" << endl;
    out << "    protocolUsed = " << flags->protocolString(false) << ";" << endl;
    out << "    httpMethodUsed = " << flags->httpMethodString() << ";" << endl;
    out << "    m_messageName = \"" << msg->methodName() << "\";" << endl;
    out << "    m_targetNamespace = \"" << msg->targetNamespace() << "\";" << endl;
    out << "}" << endl;
    // EOF (SOAP message)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal
    \fn MessageGenerator::createMessageHeader(QWebServiceMethod *msg)
  */
bool MessageGenerator::createMessageHeader(QWebServiceMethod *msg)
{
    QString msgName = msg->methodName();
    QFile file(workingDir.path() + "/" + msgName + ".h");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open message header file for writing.");

    // Possible problem in case of multi-return.
    QString msgReplyName = msg->returnValueName().first();

    QString msgReplyType = "";
    QString msgParameters = "";
    {
        // Create msgReplyType
        QMap<QString, QVariant> tempMap = msg->returnValueNameType();

        foreach (QString s, tempMap.keys()) {
            msgReplyType += tempMap.value(s).typeName();
            break;
        }

        tempMap.clear();
        tempMap = msg->parameterNamesTypes();

        // Create msgParameters (comma separated list)
        foreach (QString s, tempMap.keys()) {
            msgParameters += QString(tempMap.value(s).typeName()) + " " + s + ", ";
        }
        msgParameters.chop(2);
    }

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    // Might break on curious names
    out << "#ifndef " << msgName.toUpper() << "_H" << endl;
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
    out << "    Q_FLAGS(Protocols)" << endl;
    out << "    Q_ENUMS(HttpMethod)" << endl;
    out << endl;
    out << "public:" << endl;
    out << "    enum Protocol" << endl << "    {" << endl;
    out << "        http   = 0x01," << endl;
    out << "        soap10 = 0x02," << endl;
    out << "        soap12 = 0x04," << endl;
    out << "        soap   = 0x06," << endl;
    out << "        json   = 0x08," << endl;
    out << "        xml    = 0x10," << endl;
    out << "        rest   = 0x20" << endl;
    out << "    };" << endl;
    out << "    Q_DECLARE_FLAGS(Protocols, Protocol)" << endl;
    out << endl;
    out << "    enum HttpMethod" << endl;
    out << "    {" << endl;
    out << "        POST    = 0x1," << endl;
    out << "        GET     = 0x2," << endl;
    out << "        PUT     = 0x4," << endl;
    out << "        DELETE  = 0x8" << endl;
    out << "    };" << endl;
    out << endl;
    out << "    explicit " << msgName << "(QObject *parent = 0);" << endl;
    if (msgParameters != "") {
        out << "    " << msgName << "(" << msgParameters
            << ", QObject *parent = 0);" << endl;
    }
    out << "    ~" << msgName << "();" << endl;
    out << endl;
    out << "    void setHost(QString newHost);" << endl;
    out << "    void setHost(QUrl newHost);" << endl;
    out << "    void setMessageName(QString newName);" << endl;
    out << "    void setTargetNamespace(QString tNamespace);" << endl;
    if (!(flags->flags() & Flags::compactMode))
        out << "    void setProtocol(Protocol protocol);" << endl;
    out << "    void setHttpMethod(HttpMethod method);" << endl;
    out << "    void setParameters(" << msgParameters << ");" << endl << endl;
    out << "    bool sendMessage();" << endl;
    if ((msgParameters != "") && !((flags->flags() & Flags::compactMode)
                                   && (flags->flags() & Flags::synchronous)))
        out << "    bool sendMessage(" << msgParameters << ");" << endl;
    // Temporarily, all messages will return QString!
//    out << "    " << msgReplyType << " static sendMessage(QObject *parent";
    if (!((flags->flags() & Flags::compactMode)
          && (flags->flags() & Flags::asynchronous)))
    {
        out << "    QString static sendMessage(QObject *parent";
        if (msgParameters != "") {
            out << "," << endl;
            out << "                                " << msgParameters << ");" << endl;
        }
        else {
            out << ");" << endl;
        }
    }
    // Temporarily, all messages will return QString!
//    out << "    " << msgReplyType << " replyRead();" << endl;
    out << "    QString replyRead();" << endl;
    out << "    QString messageName() const;" << endl;
    out << "    QStringList parameterNames() const;" << endl;
    out << "    QString returnValueName() const;" << endl;
    out << "    QMap<QString, QVariant> parameterNamesTypes() const;" << endl;
    out << "    QString returnValueNameType() const;" << endl;
    out << "    QString targetNamespace() const;" << endl;
    out << "    QString host() const;" << endl;
    out << "    QUrl hostUrl() const;" << endl;
    out << "    Protocol protocol() const;" << endl;
    out << "    QString protocolString(bool includeRest) const;" << endl;
    out << "    HttpMethod httpMethod() const;" << endl;
    out << "    QString httpMethodString() const;" << endl;
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
    out << "    void prepareRequestData();" << endl;
    out << "    QString convertReplyToUtf(QString textToConvert);" << endl;
    out << endl;
    out << "    bool replyReceived;" << endl;
    out << "    Protocol protocolUsed;" << endl;
    out << "    HttpMethod httpMethodUsed;" << endl;
    out << "    QUrl hostUrlUsed;" << endl;
    out << "    QString messageNameUsed;" << endl;
    out << "    QString targetNamespaceUsed;" << endl;
    // Temporarily, all messages will return QString!
//    out << "    " << msgReplyType << " reply;" << endl;
    out << "    QString reply;" << endl;
    { // Create parameters list in declarative form.
        out << "    // -------------------------" << endl
            << "    // Parameters:" << endl;

        QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
            out << "    " << tempMap.value(s).typeName()
                << " " << s  << ";" << endl;
        }
        out << "    // End of parameters." << endl
            << "    // -------------------------" << endl;
    }
    out << "    " << msgReplyType << " " << msgReplyName << ";" << endl;
    out << "    QNetworkAccessManager *manager;" << endl;
    out << "    QNetworkReply *networkReply;" << endl;
    out << "    QByteArray data;" << endl;
    out << "};" << endl << endl;
    out << "Q_DECLARE_OPERATORS_FOR_FLAGS(" << msgName << "::Protocols)" << endl;
    out << "#endif // " << msgName.toUpper() << "_H" << endl;
    // EOF (SOAP message)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal
    \fn MessageGenerator::createMessageSource(QWebServiceMethod *msg)
  */
bool MessageGenerator::createMessageSource(QWebServiceMethod *msg)
{
    QString msgName = msg->methodName();
    QFile file(workingDir.path() + "/" + msgName + ".cpp");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open message source file for writing.");

    // Possible problem in case of multi-return.
    QString msgReplyName = msg->returnValueName().first();

    QString msgReplyType = "";
    QString msgParameters = "";
    {
        QMap<QString, QVariant> tempMap = msg->returnValueNameType();

        foreach (QString s, tempMap.keys()) {
            msgReplyType += tempMap.value(s).typeName();
            break;
        }

        tempMap.clear();
        tempMap = msg->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
            msgParameters += QString(tempMap.value(s).typeName())
                    + " " + s + ", ";
        }
        msgParameters.chop(2);
    }

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "#include \"";
    if (!(flags->flags() & Flags::allInOneDirStructure))
        out << "../headers/";
    out << msgName << ".h\"" << endl;
    out << endl;
    out << msgName << "::" << msgName << "(QObject *parent) :" << endl;
    out << "    QObject(parent)" << endl;
    out << "{" << endl;

    if (msg->host() != "")
        out << "    hostUrlUsed.setHost(\"" << msg->host() << "\");" << endl;
    else
        out << "    hostUrlUsed.setHost(\"" << msg->targetNamespace()
            << "\");" << endl;

    out << "    messageNameUsed = \"" << msgName << "\";" << endl;
    out << "    replyReceived = false;" << endl;
    out << "    reply.clear();" << endl;
    out << "    connect(manager, SIGNAL(finished(QNetworkReply*)), "
        << "this, SLOT(replyFinished(QNetworkReply*)));" << endl;
    { // Defaulting the protocol:
        out << "    protocolUsed = " << flags->protocolString() << ";" << endl;
    }

    out << "}" << endl;
    out << endl;
    if (msgParameters != "") {
        out << msgName << "::" << msgName << "(" << msgParameters
            << ", QObject *parent) :" << endl;
        out << "    QObject(parent)" << endl;
        out << "{" << endl;
        { // Assign all parameters.
            assignAllParameters(msg, out);

            // Defaulting the protocol:
            out << "    protocolUsed = " << flags->protocolString() << ";" << endl;
        }

        if (msg->host() != "") {
            out << "    hostUrlUsed.setHost(\"" << msg->host() << "\");" << endl;
        }
        else {
            out << "    hostUrlUsed.setHost(\"" << msg->targetNamespace()
                << "\");" << endl;
        }

        out << "    messageNameUsed = \"" << msgName << "\";" << endl;
        out << "    replyReceived = false;" << endl;
        out << "    reply.clear();" << endl;
        out << "    connect(manager, SIGNAL(finished(QNetworkReply*)), "
            << "this, SLOT(replyFinished(QNetworkReply*)));" << endl;
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
    out << "void " << msgName << "::setParameters(" << msgParameters << ")" << endl;
    out << "{" << endl;

    assignAllParameters(msg, out);

    out << "}" << endl;
    out << endl;

    out << "void " << msgName << "::setHost(QString newHost)" << endl;
    out << "{" << endl;
    out << "    hostUrlUsed.setHost(newHost);" << endl;
    out << "}" << endl;
    out << endl;
    out << "void " << msgName << "::setHost(QUrl newHost)" << endl;
    out << "{" << endl;
    out << "    hostUrlUsed = newHost;" << endl;
    out << "}" << endl;
    out << endl;
    out << "void " << msgName << "::setMessageName(QString newName)" << endl;
    out << "{" << endl;
    out << "    messageNameUsed = newName;" << endl;
    out << "}" << endl;
    out << endl;
    out << "void " << msgName << "::setTargetNamespace(QString tNamespace)" << endl;
    out << "{" << endl;
    out << "    targetNamespaceUsed = tNamespace;" << endl;
    out << "}" << endl;
    out << endl;
    if (!(flags->flags() & Flags::compactMode)) {
        out << "void " << msgName << "::setProtocol(Protocol prot)" << endl;
        out << "{" << endl;
        out << "    // Prevent incompatibile flags from being set simultaneously:"
            << endl;
        out << "    QList<int> allowedCombinations;" << endl;
        out << "    allowedCombinations << 0x01 << 0x02 << 0x04 << 0x06 "
            << "<< 0x08 << 0x10 << 0x20; // Standard values." << endl;
        out << "    allowedCombinations << 0x21 << 0x22 << 0x24 << 0x26 "
            << "<< 0x28 << 0x30; // REST combinations" << endl;
        out << "" << endl;
        out << "    if (allowedCombinations.contains(prot)) {" << endl;
        out << "        if (prot & soap)" << endl;
        out << "            protocolUsed = soap12;" << endl;
        out << "        else" << endl;
        out << "            protocolUsed = prot;" << endl;
        out << "    }" << endl;
        out << "    else {" << endl;
        out << "        protocolUsed = soap12;" << endl;
        out << "    }" << endl;
        out << "}" << endl;
        out << endl;
    }
    out << "void " << msgName << "::setHttpMethod(HttpMethod method)" << endl;
    out << "{" << endl;
    out << "    httpMethodUsed = method;" << endl;
    out << "}" << endl;
    out << endl;
    out << "bool " << msgName << "::sendMessage()" << endl;
    out << "{" << endl;
    out << "    QNetworkRequest request;" << endl;
    out << "    request.setUrl(hostUrlUsed);" << endl;
    out << "    if (protocolUsed & soap)" << endl;
    out << "        request.setHeader(QNetworkRequest::ContentTypeHeader, "
        << "QVariant(\"application/soap+xml; charset=utf-8\"));" << endl;
    out << "    else if (protocolUsed == json)" << endl;
    out << "        request.setHeader(QNetworkRequest::ContentTypeHeader, "
        << "QVariant(\"application/json; charset=utf-8\"));" << endl;
    out << "    else if (protocolUsed == http)" << endl;
    out << "        request.setHeader(QNetworkRequest::ContentTypeHeader, "
        << "QVariant(\"Content-Type: application/x-www-form-urlencoded\"));" << endl;
    out << "    else if (protocolUsed & xml)" << endl;
    out << "        request.setHeader(QNetworkRequest::ContentTypeHeader, "
        << "QVariant(\"application/xml; charset=utf-8\"));" << endl;
    out << endl;
    out << "    if (protocolUsed == soap10)" << endl;
    out << "        request.setRawHeader(QByteArray(\"SOAPAction\"), "
        << "QByteArray(hostUrlUsed.toString().toAscii()));" << endl;
    out << endl;
    out << "    prepareRequestData();" << endl;
    out << endl;
    if (flags->flags() & Flags::debugMode) {
        out << "    qDebug() << request.rawHeaderList() << \" \" "
            << "<< request.url().toString();" << endl;
        out << "    qDebug() << \"*************************\";" << endl;
        out << endl;
    }    
    out << "    if (protocolUsed & rest) {" << endl;
    out << "        if (httpMethodUsed == POST)" << endl;
    out << "            manager->post(request, data);" << endl;
    out << "        else if (httpMethodUsed == GET)" << endl;
    out << "            manager->get(request);" << endl;
    out << "        else if (httpMethodUsed == PUT)" << endl;
    out << "            manager->put(request, data);" << endl;
    out << "        else if (httpMethodUsed == DELETE)" << endl;
    out << "            manager->deleteResource(request);" << endl;
    out << "    }" << endl;
    out << "    else {" << endl;
    out << "        manager->post(request, data);" << endl;
    out << "    }    " << endl;
    out << "    return true;" << endl;
    out << "}" << endl;
    out << endl;
    if ((msgParameters != "")
            && !((flags->flags() & Flags::compactMode)
                 && (flags->flags() & Flags::synchronous))) {
        out << "bool " << msgName << "::sendMessage("
            << msgParameters << ")" << endl;
        out << "{" << endl;

        assignAllParameters(msg, out);

        out << "    sendMessage();" << endl;
        out << "    return true;" << endl;
        out << "}" << endl;
        out << endl;
    }
    if (!((flags->flags() & Flags::compactMode)
          && (flags->flags() & Flags::asynchronous))) {
        out << "/* STATIC */" << endl;
        // Temporarily, all messages will return QString!
        //    out << "" << msgReplyType << " " << msgName
        // << "::sendMessage(QObject *parent, " << msgParameters << ")" << endl;
        out << "QString " << msgName << "::sendMessage(QObject *parent";
        if (msgParameters != "")
            out << ", " << msgParameters;
        out << ")" << endl;
        out << "{" << endl;
        out << "    " << msgName << " qsm(parent);" << endl;
        { // Assign all parameters.
            out << "    qsm.setParameters(";
            QString tempS = "";
            QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();

            foreach (QString s, tempMap.keys()) {
                tempS += s + ", ";
            }
            tempS.chop(2);
            out << tempS << ");" << endl;
        }

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
    }
    // Temporarily, all messages will return QString!
//    out << "" << msgReplyType << " " << msgName << "::replyRead()" << endl;
    out << "QString " << msgName << "::replyRead()" << endl;
    out << "{" << endl;
    out << "    return reply;" << endl;
    out << "}" << endl;
    out << endl;
    out << "QString " << msgName << "::messageName() const" << endl;
    out << "{" << endl;
    out << "    return messageNameUsed;" << endl;
    out << "}" << endl;
    out << endl;
    out << "QStringList " << msgName << "::parameterNames() const" << endl;
    out << "{" << endl;
    out << "    QMap<QString, QVariant> parameters;" << endl;
    { // Assign all parameters.
        QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
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
    out << "QString " << msgName << "::returnValueName() const" << endl;
    out << "{" << endl;
    out << "    return \"" << msgReplyName << "\";" << endl;
    out << "}" << endl;
    out << endl;
    out << "QMap<QString, QVariant> " << msgName
        << "::parameterNamesTypes() const" << endl;
    out << "{" << endl;
    out << "    QMap<QString, QVariant> parameters;" << endl;
    { // Assign all parameters.
        QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
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
    out << "QString " << msgName << "::returnValueNameType() const" << endl;
    out << "{" << endl;
    out << "    return \"" << msgReplyType << "\";" << endl;
    out << "}" << endl;
    out << endl;
    out << "QString " << msgName << "::targetNamespace() const" << endl;
    out << "{" << endl;
    out << "    return targetNamespaceUsed;" << endl;
    out << "}" << endl;
    out << endl;

    out << "QString " << msgName << "::host() const" << endl;
    out << "{" << endl;
    out << "    return hostUrlUsed.host();" << endl;
    out << "}" << endl;
    out << endl;
    out << "QUrl " << msgName << "::hostUrl() const" << endl;
    out << "{" << endl;
    out << "    return hostUrlUsed;" << endl;
    out << "}" << endl;
    out << endl;
    out << msgName << "::Protocol " << msgName << "::protocol() const" << endl;
    out << "{" << endl;
    out << "    return protocolUsed;" << endl;
    out << "}" << endl;
    out << endl;
    out << msgName << "::HttpMethod " << msgName << "::httpMethod() const" << endl;
    out << "{" << endl;
    out << "    return httpMethodUsed;" << endl;
    out << "}" << endl;
    out << endl;
    out << "QString " << msgName << "::protocolString(bool includeRest) const" << endl;
    out << "{" << endl;
    out << "    QString result = \"\";" << endl;
    out << endl;
    out << "    if (protocolUsed & http)" << endl;
    out << "        result = \"http\";" << endl;
    out << "    else if (protocolUsed & soap10)" << endl;
    out << "        result = \"soap10\";" << endl;
    out << "    else if (protocolUsed & soap12)" << endl;
    out << "        result = \"soap12\";" << endl;
    out << "    else if (protocolUsed & json)" << endl;
    out << "        result = \"json\";" << endl;
    out << "    else if (protocolUsed & xml)" << endl;
    out << "        result = \"xml\";" << endl;
    out << endl;
    out << "    if (includeRest && (protocolUsed & rest))" << endl;
    out << "        result += \",rest\";" << endl;
    out << endl;
    out << "    return result;" << endl;
    out << "}" << endl;
    out << endl;
    out << "QString " << msgName << "::httpMethodString() const" << endl;
    out << "{" << endl;
    out << "    QString result = \"\";" << endl;
    out << endl;
    out << "    if (httpMethodUsed == POST)" << endl;
    out << "        result = \"POST\";" << endl;
    out << "    else if (httpMethodUsed == GET)" << endl;
    out << "        result = \"GET\";" << endl;
    out << "    else if (httpMethodUsed == PUT)" << endl;
    out << "        result = \"PUT\";" << endl;
    out << "    else if (httpMethodUsed == DELETE)" << endl;
    out << "        result = \"DELETE\";" << endl;
    out << endl;
    out << "    return result;" << endl;
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
    out << "    QString tempBegin = \"<\" + messageNameUsed + \"Result>\";" << endl;
    out << "    int replyBeginIndex = replyString.indexOf(tempBegin, 0, "
        << "Qt::CaseSensitive);" << endl;
    out << "    replyBeginIndex += tempBegin.length();" << endl;
    out << endl;
    out << "    QString tempFinish = \"</\" + messageNameUsed + \"Result>\";" << endl;
    out << "    int replyFinishIndex = replyString.indexOf(tempFinish, "
        << "replyBeginIndex, Qt::CaseSensitive);" << endl;
    out << endl;
    out << "    if (replyBeginIndex == -1)" << endl;
    out << "        replyBytes = 0;" << endl;
    out << "    if (replyFinishIndex == -1)" << endl;
    out << "        replyFinishIndex = replyString.length();" << endl;
    out << endl;
    // Temporarily, all messages will return QString!
    out << "    reply = (QString) replyString.mid(replyBeginIndex, "
        << "replyFinishIndex - replyBeginIndex);" << endl;
    out << endl;
    out << "    replyReceived = true;" << endl;
    out << "    emit replyReady(reply);" << endl;
    out << "}" << endl;
    out << endl;
    out << "void " << msgName << "::prepareRequestData()" << endl;
    out << "{" << endl;
    out << "    data.clear();" << endl;
    out << "    QString header, body, footer;" << endl;
    out << "    QString endl = \"\\r\\n\";" << endl;
    out << "    QMap<QString, QVariant> parameters = parameterNamesTypes();" << endl;
    out << endl;
    out << "    if (protocolUsed & soap)" << endl;
    out << "    {" << endl;
    out << "        if (protocolUsed == soap12)" << endl;
    out << "        {" << endl;
    out << "            header = \"<?xml version=\\\"1.0\\\" "
        << "encoding=\\\"utf-8\\\"?> \" + endl +" << endl;
    out << "                 \"<soap12:Envelope "
        << "xmlns:xsi=\\\"http://www.w3.org/2001/XMLSchema-instance\\\" \" +" << endl;
    out << "                 "
        << "\"xmlns:xsd=\\\"http://www.w3.org/2001/XMLSchema\\\" \" +" << endl;
    out << "                 "
        << "\"xmlns:soap12=\\\"http://www.w3.org/2003/05/soap-envelope\\\"> \""
        << " + endl +" << endl;
    out << "                 \"<soap12:Body> \" + endl;" << endl;
    out << endl;
    out << "            footer = \"</soap12:Body> \" + endl + "
        << "\"</soap12:Envelope>\";" << endl;
    out << "        }" << endl;
    out << "        else if (protocolUsed == soap10)" << endl;
    out << "        {" << endl;
    out << "            "
        << "header = \"<?xml version=\\\"1.0\\\" encoding=\\\"utf-8\\\"?> \" + endl +" << endl;
    out << "                "
        << "\"<soap:Envelope xmlns:xsi=\\\"http://www.w3.org/2001/XMLSchema-instance\\\" \" +" << endl;
    out << "                "
        << "\"xmlns:xsd=\\\"http://www.w3.org/2001/XMLSchema\\\" \" +" << endl;
    out << "                "
        << "\"xmlns:soap=\\\"http://www.w3.org/2003/05/soap-envelope\\\"> \" + endl +" << endl;
    out << "                \"<soap:Body> \" + endl;" << endl;
    out << endl;
    out << "            footer = \"</soap:Body> \" + endl + \"</soap:Envelope>\";" << endl;
    out << "        }" << endl;
    out << endl;
    out << "        body = \"\\t<\" + messageNameUsed + \" xmlns=\\\"\" "
        << "+ targetNamespaceUsed + \"\\\"> \" + endl;" << endl;
    out << endl;
    out << "        foreach (const QString currentKey, parameters.keys())" << endl;
    out << "        {" << endl;
    out << "            QVariant qv = parameters.value(currentKey);" << endl;
    out << "            // Currently, this does not handle nested lists" << endl;
    out << "            body += \"\\t\\t<\" + currentKey + \">\" + qv.toString() "
        << "+ \"</\" + currentKey + \"> \" + endl;" << endl;
    out << "        }" << endl;
    out << endl;
    out << "        body += \"\\t</\" + messageNameUsed + \"> \" + endl;" << endl;
    out << "    }" << endl;
    out << "    else if (protocolUsed == http)" << endl;
    out << "    {" << endl;
    out << "        foreach (const QString currentKey, parameters.keys())" << endl;
    out << "        {" << endl;
    out << "            QVariant qv = parameters.value(currentKey);" << endl;
    out << "            // Currently, this does not handle nested lists" << endl;
    out << "            body += currentKey + \"=\" + qv.toString() + \"&\";" << endl;
    out << "        }" << endl;
    out << "        body.chop(1);" << endl;
    out << "    }" << endl;
    out << "    else if (protocolUsed == json)" << endl;
    out << "    {" << endl;
    out << "        body += \"{\" + endl;" << endl;
    out << "        foreach (const QString currentKey, parameters.keys())" << endl;
    out << "        {" << endl;
    out << "            QVariant qv = parameters.value(currentKey);" << endl;
    out << "            // Currently, this does not handle nested lists" << endl;
    out << "            body += \"{\" + endl + \"\\t\\\"\" + currentKey + \"\\\" "
        << ": \\\"\" + qv.toString() + \"\\\"\" + endl;"
        << endl;
    out << "        }" << endl;
    out << "        body += \"}\";" << endl;
    out << "    }" << endl;
    out << "    else if (protocolUsed & xml) {" << endl;
    out << "        foreach (const QString currentKey, parameters.keys()) {" << endl;
    out << "            QVariant qv = parameters.value(currentKey);" << endl;
    out << "            // Currently, this does not handle nested lists" << endl;
    out << "            body += \"\\t\\t<\" + currentKey + \">\" + qv.toString() "
        << "+ \"</\" + currentKey + \"> \" + endl;" << endl;
    out << "        }" << endl;
    out << "    }" << endl;
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

    Creates a dummy main.cpp file. It's needed only for successful compilation
    of freshly generated code. It is NOT NEEDED for any other reason.
    You can safely delete it fo your project.
  */
bool MessageGenerator::createMainCpp()
{
    QFile file(workingDir.path() + "/main.cpp");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open Web Service header file for writing.");

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "/*Creates a dummy main.cpp file. It's needed only for successful "
        << "compilation of freshly generated code. It is NOT NEEDED for any "
        << "other reason. You can safely delete it fo your project (just "
        << "remember to remove it from .pro file, too). */" << endl;
    out << "#include \"../headers/band_ws.h\"" << endl;
    out << "int main() {return 0;}" << endl;
    // EOF (main.cpp)
    // ---------------------------------

    file.close();
    return true;
}

/*!
  \internal

  Assigns all message parameters to this-><paramName>.
  */
void MessageGenerator::assignAllParameters(QWebServiceMethod *msg, QTextStream &out)
{
    QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();

    foreach (QString s, tempMap.keys()) {
        out << "    this->" << s << " = " << s << ";" << endl;
    }
}
