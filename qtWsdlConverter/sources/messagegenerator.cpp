#include "../headers/messagegenerator.h"

/*!
    \class MessageGenerator
    \brief Creates code for messages.

    Creates messages (both headers and sources) by generating the entire code
    or subclassing QWebMethod (depending on whether --subclass flag is set).
  */

/*!
    \fn MessageGenerator::MessageGenerator(QMap<QString, QWebMethod *> *msgs, QDir wrkDir, Flags *flgs, QObject *parent)

    Constructs QObject using \a parent, initialises MessageGenerator with messages (\a msgs),
    working directory (\a wrkDir), and flags (\a flgs).
  */
MessageGenerator::MessageGenerator(QMap<QString, QWebMethod *> *msgs,
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
    m_errorMessage += errMessage + " ";
    return false;
}

/*!
    \fn MessageGenerator::createMessages()

    Creates messages (both headers and sources) by generating the entire code
    or subclassing QWebMethod (depending on whether --subclass flag is set).

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
        QWebMethod *m = messages->value(s);

        if (flags->flags() & Flags::subclass) {
            if (!createSubclassedMessageHeader(m))
                return enterErrorState("Creating header for message \"" + m->messageName() + "\" failed!");
        }
        else {
            if (!createMessageHeader(m))
                return enterErrorState("Creating header for message \"" + m->messageName() + "\" failed!");
        }
    }

    if (!(flags->flags() & Flags::allInOneDirStructure)) {
        workingDir.cdUp();
        workingDir.cd("sources");
    }

    foreach (QString s, messages->keys()) {
        QWebMethod *n = messages->value(s);

        if (flags->flags() & Flags::subclass) {
            if (!createSubclassedMessageSource(n))
                return enterErrorState("Creating header for message \"" + n->messageName() + "\" failed!");
        }
        else {
            if (!createMessageSource(n))
                return enterErrorState("Creating source for message \"" + n->messageName() + "\" failed!");
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
    \fn MessageGenerator::createSubclassedMessageHeader(QWebMethod *msg)
  */
bool MessageGenerator::createSubclassedMessageHeader(QWebMethod *msg)
{
    QString msgName = msg->messageName();
    QFile file(workingDir.path() + "/" + msgName + ".h");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open message header file for writing.");

    QString msgReplyName = msg->returnValueName().first(); // Possible problem in case of multi-return.

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
    out << "#ifndef " << msgName.toUpper() << "_H" << endl; // might break on curious names
    out << "#define " << msgName.toUpper() << "_H" << endl;
    out << endl;
    out << "#include <QWebService>" << endl; // Should be <QWebService/qwebmethod.h>
    out << endl;
    out << "class " << msgName << " : public QWebMethod" << endl;
    out << "{" << endl;
    out << "    Q_OBJECT" << endl;
    out << endl;
    /*
       Add async and static sendMessage() methods with QString returns.
       Optionally, also add a constructor.
       Reimplement configure().
    */
    out << "};" << endl;
    // EOF (SOAP message)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal
    \fn MessageGenerator::createSubclassedMessageSource(QWebMethod *msg)
  */
bool MessageGenerator::createSubclassedMessageSource(QWebMethod *msg)
{
    QString msgName = msg->messageName();
    QFile file(workingDir.path() + "/" + msgName + ".cpp");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open message source file for writing.");

    QString msgReplyName = msg->returnValueName().first(); // Possible problem in case of multi-return.

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
            msgParameters += QString(tempMap.value(s).typeName()) + " " + s + ", ";
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
    /*
       Add async and static sendMessage() methods with QString returns.
       Optionally, also add a constructor.
       Reimplement configure().
    */
    out << "void " << msgName << "::configure()" << endl;
    out << "{" << endl;
    // Add body here
    out << "}" << endl;
    // EOF (SOAP message)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal
    \fn MessageGenerator::createMessageHeader(QWebMethod *msg)
  */
bool MessageGenerator::createMessageHeader(QWebMethod *msg)
{
    QString msgName = msg->messageName();
    QFile file(workingDir.path() + "/" + msgName + ".h");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open message header file for writing.");

    QString msgReplyName = msg->returnValueName().first(); // Possible problem in case of multi-return.

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
    if (msgParameters != "")
        out << "    " << msgName << "(" << msgParameters << ", QObject *parent = 0);" << endl;
    out << "    ~" << msgName << "();" << endl;
    out << endl;
    out << "    void setParams(" << msgParameters << ");" << endl;
    if (!(flags->flags() & Flags::compactMode))
        out << "    void setProtocol(Protocol protocol);" << endl;
    out << "    bool sendMessage();" << endl;
    if ((msgParameters != "") && !((flags->flags() & Flags::compactMode) && (flags->flags() & Flags::synchronous)))
        out << "    bool sendMessage(" << msgParameters << ");" << endl;
    // Temporarily, all messages will return QString!
//    out << "    " << msgReplyType << " static sendMessage(QObject *parent";
    if (!((flags->flags() & Flags::compactMode) && (flags->flags() & Flags::asynchronous)))
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
    out << "    void prepareRequestData();" << endl;
    out << "    QString convertReplyToUtf(QString textToConvert);" << endl;
    out << endl;
    out << "    bool replyReceived;" << endl;
    out << "    Protocol protocol;" << endl;
    out << "    QUrl hostUrl;" << endl;
    out << "    QString messageName;" << endl;
    out << "    QString targetNamespace;" << endl;
    // Temporarily, all messages will return QString!
//    out << "    " << msgReplyType << " reply;" << endl;
    out << "    QString reply;" << endl;
    { // Create parameters list in declarative form.
        out << "    // -------------------------" << endl << "    // Parameters:" << endl;
        QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
            out << "    " << tempMap.value(s).typeName() << " " << s  << ";" << endl;
        }
        out << "    // End of parameters." << endl << "    // -------------------------" << endl;
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
    \fn MessageGenerator::createMessageSource(QWebMethod *msg)
  */
bool MessageGenerator::createMessageSource(QWebMethod *msg)
{
    QString msgName = msg->messageName();
    QFile file(workingDir.path() + "/" + msgName + ".cpp");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open message source file for writing.");

    QString msgReplyName = msg->returnValueName().first(); // Possible problem in case of multi-return.

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
            msgParameters += QString(tempMap.value(s).typeName()) + " " + s + ", ";
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
        out << "    hostUrl.setHost(\"" << msg->host() << "\");" << endl;
    else
        out << "    hostUrl.setHost(\"" << msg->targetNamespace() << "\");" << endl;

    out << "    messageName = \"" << msgName << "\";" << endl;
    out << "    replyReceived = false;" << endl;
    out << "    reply.clear();" << endl;
    out << "    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));" << endl;
    { // Defaulting the protocol:
        out << "    protocol = " << flags->protocolString() << ";" << endl;
    }

    out << "}" << endl;
    out << endl;
    if (msgParameters != "") {
        out << msgName << "::" << msgName << "(" << msgParameters << ", QObject *parent) :" << endl;
        out << "    QObject(parent)" << endl;
        out << "{" << endl;
        { // Assign all parameters.
            QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();

            foreach (QString s, tempMap.keys()) {
                out << "    this->" << s << " = " << s << ";" << endl;
            }

            // Defaulting the protocol:
            out << "    protocol = " << flags->protocolString() << ";" << endl;
        }

        if (msg->host() != "")
            out << "    hostUrl.setHost(\"" << msg->host() << "\");" << endl;
        else
            out << "    hostUrl.setHost(\"" << msg->targetNamespace() << "\");" << endl;
//        out << "    hostUrl.setHost(host + messageName);" << endl; // This is probably wrong, vars are not set!

        out << "    messageName = \"" << msgName << "\";" << endl;
        out << "    replyReceived = false;" << endl;
        out << "    reply.clear();" << endl;
        out << "    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));" << endl;
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
        QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
            out << "    this->" << s << " = " << s << ";" << endl;
        }
    }
    out << "}" << endl;
    out << endl;
    if (!(flags->flags() & Flags::compactMode)) {
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
    if (flags->flags() & Flags::debugMode) {
        out << "    qDebug() << request.rawHeaderList() << \" \" << request.url().toString();" << endl;
        out << "    qDebug() << \"*************************\";" << endl;
        out << endl;
    }
    out << "    manager->post(request, data);" << endl;
    out << "    return true;" << endl;
    out << "}" << endl;
    out << endl;
    if ((msgParameters != "") && !((flags->flags() & Flags::compactMode) && (flags->flags() & Flags::synchronous))) {
        out << "bool " << msgName << "::sendMessage(" << msgParameters << ")" << endl;
        out << "{" << endl;
        { // Assign all parameters.
            QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();
            foreach (QString s, tempMap.keys()) {
                out << "    this->" << s << " = " << s << ";" << endl;
            }
        }
        out << "    sendMessage();" << endl;
        out << "    return true;" << endl;
        out << "}" << endl;
        out << endl;
    }
    if (!((flags->flags() & Flags::compactMode) && (flags->flags() & Flags::asynchronous))) {
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
    out << "QString " << msgName << "::getMessageName()" << endl;
    out << "{" << endl;
    out << "    return messageName;" << endl;
    out << "}" << endl;
    out << endl;
    out << "QStringList " << msgName << "::getParameterNames() const" << endl;
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
    out << "QString " << msgName << "::getReturnValueName() const" << endl;
    out << "{" << endl;
    out << "    return \"" << msgReplyName << "\";" << endl;
    out << "}" << endl;
    out << endl;
    out << "QMap<QString, QVariant> " << msgName << "::getParameterNamesTypes() const" << endl;
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
    out << "void " << msgName << "::prepareRequestData()" << endl;
    out << "{" << endl;
    out << "    data.clear();" << endl;
    out << "    QString header, body, footer;" << endl;
    out << "    QString endl = \"\\r\\n\"; // Replace with something OS-independent, or seriously rethink." << endl;
    out << "    QMap<QString, QVariant> parameters = getParameterNamesTypes();" << endl;
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
bool MessageGenerator::createMainCpp()
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
