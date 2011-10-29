/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the qtWsdlConverter tool.
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.txt included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#include "../headers/messagegenerator.h"

/*!
    \class MessageGenerator
    \brief Creates code for messages.

    Creates messages (both headers and sources) by generating the entire code
    or subclassing QWebServiceMethod (depending on whether --subclass flag is set).
  */

/*!
    Constructs QObject using \a parent, initialises MessageGenerator
    with messages (\a mthds), working directory (\a wrkDir), and flags (\a flgs).
  */
MessageGenerator::MessageGenerator(QMap<QString, QWebServiceMethod *> *mthds,
                                   const QDir &wrkDir,
                                   Flags *flgs, QObject *parent) :
    QObject(parent), methods(mthds), workingDir(wrkDir), flags(flgs)
{
    errorState = false;
    logic = TemplateLogic(flags);
}

/*!
  Returns QString containing error message (or "" if there was no error).
  */
QString MessageGenerator::errorMessage()
{
    return m_errorMessage;
}

/*!
    \internal
  */
bool MessageGenerator::enterErrorState(const QString errMessage)
{
    m_errorMessage += errMessage + QLatin1String("\n");
    return false;
}

/*!
    Creates messages (both headers and sources) by generating the entire code
    or subclassing QWebServiceMethod (depending on whether --subclass flag is set).

    Returns true if successful.
  */
bool MessageGenerator::createMessages()
{
    if (flags->flags() & Flags::NoMessagesStructure) {
        if (!(flags->flags() & Flags::AllInOneDirStructure)) {
            workingDir.cd(QLatin1String("sources"));
            createMainCpp();
            workingDir.cdUp();
        } else {
            createMainCpp();
        }

        return true;
    }

    if (!(flags->flags() & Flags::AllInOneDirStructure))
        workingDir.cd(QLatin1String("headers"));

    foreach (QString s, methods->keys()) {
        QWebServiceMethod *m = methods->value(s);

        if (flags->flags() & Flags::Subclass) {
            if (!createSubclassedMessageHeader(m))
                return enterErrorState("Creating header for message \""
                                       + m->methodName() + "\" failed!");
        } else {
            if (!createMessageHeader(m))
                return enterErrorState("Creating header for message \""
                                       + m->methodName() + "\" failed!");
        }
    }

    if (!(flags->flags() & Flags::AllInOneDirStructure)) {
        workingDir.cdUp();
        workingDir.cd(QLatin1String("sources"));
    }

    foreach (QString s, methods->keys()) {
        QWebServiceMethod *n = methods->value(s);

        if (flags->flags() & Flags::Subclass) {
            if (!createSubclassedMessageSource(n))
                return enterErrorState("Creating header for message \""
                                       + n->methodName() + "\" failed!");
        } else {
            if (!createMessageSource(n))
                return enterErrorState("Creating source for message \""
                                       + n->methodName() + "\" failed!");
        }
    }

    if (!(flags->flags() & Flags::AllInOneDirStructure)) {
        createMainCpp();
        workingDir.cdUp();
    }

    return true;
}

/*!
    \internal

    Creates a subclassed web method header.
  */
bool MessageGenerator::createSubclassedMessageHeader(QWebServiceMethod *msg)
{
    QString msgName = msg->methodName();
    QFile file(QString(workingDir.path() + QLatin1String("/")
                       + msgName + QLatin1String(".h")));
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState(QLatin1String("Error: could not open message "
                                             "header file for writing."));

    // Possible problem in case of multi-return.
    QString msgReplyName = msg->returnValueName().first();

    QString msgReplyType;
    QString msgParameters;
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

    QString messageHeader = logic.readFile("../qtWsdlConverter/templates/method_header");
    int beginIndex = 0;

    messageHeader.replace("%headerIfndef%", msgName.toUpper() + "_H");
    messageHeader.replace("%method%", msgName);
    messageHeader.replace("%params%", msgParameters);

    // Create custom constructor.
    if (msgParameters != QString()) {
        beginIndex = logic.removeTag(messageHeader, "%methodConstructor%");

        QString toInsert = flags->tab() + msgName + "(" + msgParameters
                + ", QObject *parent = 0);" + flags->tab();
        messageHeader.insert(beginIndex, toInsert);
        beginIndex += toInsert.length();
    }

    { // Create parameters list in declarative form.
        beginIndex = logic.removeTag(messageHeader, "%paramsDeclaration%");

        QString toInsert = flags->tab() + "// -------------------------"
                + flags->endLine() + flags->tab()
                + "// Parameters:" +flags->endLine();

        QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
            toInsert += flags->tab() + tempMap.value(s).typeName()
                    + " " + s + ";" + flags->endLine();
        }
       toInsert += flags->tab() + "// End of parameters." + flags->endLine()
               + flags->tab() + "// -------------------------" + flags->endLine();

        messageHeader.insert(beginIndex, toInsert);
        beginIndex += toInsert.length();
    }

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << messageHeader;
    // EOF (SOAP message)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal

    Creates a subclassed web method source file.
  */
bool MessageGenerator::createSubclassedMessageSource(QWebServiceMethod *msg)
{
    QString msgName = msg->methodName();
    QFile file(QString(workingDir.path() + QLatin1String("/")
                       + msgName + QLatin1String(".cpp")));
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState(QLatin1String("Error: could not open message "
                                             "source file for writing."));

    // Possible problem in case of multi-return.
    QString msgReplyName = msg->returnValueName().first();

    QString msgReplyType;
    QString msgParameters;
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

    QString messageSource = logic.readFile("../qtWsdlConverter/templates/method_source");
    int beginIndex = 0;

    messageSource.replace("%method%", msgName);
    messageSource.replace("%params%", msgParameters);
    messageSource.replace("%assignParams%", assignAllParameters(msg));

    beginIndex = logic.removeTag(messageSource, "%include%");
    QString toInsert = "#include \"";
    if (!(flags->flags() & Flags::AllInOneDirStructure))
        toInsert += "../headers/";
    toInsert += msgName + ".h\"";
    messageSource.insert(beginIndex, toInsert);

    beginIndex = logic.removeTag(messageSource, "%methodConstructor%");
    if (msgParameters != QString()) {
        toInsert = msgName + "::" + msgName + "(" + msgParameters
                + ", QObject *parent) :" + flags->endLine()
                + flags->tab() + "QWebServiceMethod(parent)" +  flags->endLine()
                + "{" +  flags->endLine()
                + assignAllParameters(msg) +  flags->tab()
                + "configure();" +  flags->endLine()
                + "}";

        messageSource.insert(beginIndex, toInsert);
    }

    beginIndex = logic.removeTag(messageSource, "%staticInvoke%");
    if (!((flags->flags() & Flags::CompactMode)
          && (flags->flags() & Flags::Asynchronous))) {
        toInsert = "/* STATIC */" + flags->endLine()
                + "QString " + msgName + "::sendMessage(QObject *parent";
        if (msgParameters != QString())
            toInsert += ", " + msgParameters;
        toInsert += ")" + flags->endLine()
                + "{" + flags->endLine()
                + flags->tab() + msgName + " qsm(";
        { // Assign all parameters.
            QString tempS;
            QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();

            foreach (QString s, tempMap.keys()) {
                tempS += s + ", ";
            }
//            tempS.chop(2);
            toInsert += tempS + "parent);" + flags->endLine();
        }
        toInsert += flags->endLine() + flags->tab()
                + "qsm.sendMessage();" + flags->endLine()
                + flags->tab() + "// TODO: ADD ERROR HANDLING!" + flags->endLine()
                + flags->tab() + "forever" + flags->endLine()
                + flags->tab() + "{" + flags->endLine()
                + flags->tab() + flags->tab() + "if (qsm.replyReceived)" + flags->endLine()
                + flags->tab() + flags->tab() + flags->tab() + "return qsm.reply.toString();"
                + flags->endLine()
                + flags->tab() + flags->tab() + "else" + flags->endLine()
                + flags->tab() + flags->tab() + "{" + flags->endLine()
                + flags->tab() + flags->tab() + flags->tab() + "qApp->processEvents();"
                + flags->endLine()
                + flags->tab() + flags->tab() + "}" + flags->endLine()
                + flags->tab() + "}" + flags->endLine()
                + "}";

        messageSource.insert(beginIndex, toInsert);
    }
    beginIndex = logic.removeTag(messageSource, "%configure%");
    toInsert = flags->tab() + "m_hostUrl = QUrl(\"" + msg->host() + "\");"
            + flags->endLine()
            + flags->tab() + "protocolUsed = " + flags->protocolString(false) + ";"
            + flags->endLine()
            + flags->tab() + "httpMethodUsed = " + flags->httpMethodString() + ";"
            + flags->endLine()
            + flags->tab() + "m_messageName = \"" + msg->methodName() + "\";"
            + flags->endLine()
            + flags->tab() + "m_targetNamespace = \"" + msg->targetNamespace() + "\";";
    messageSource.insert(beginIndex, toInsert);

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << messageSource;
    // EOF (SOAP message)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal
  */
bool MessageGenerator::createMessageHeader(QWebServiceMethod *msg)
{
    QString msgName = msg->methodName();
    QFile file(QString(workingDir.path() + QLatin1String("/")
                       + msgName + QLatin1String(".h")));
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState(QLatin1String("Error: could not open message "
                                             "header file for writing."));

    // Possible problem in case of multi-return.
    QString msgReplyName = msg->returnValueName().first();

    QString msgReplyType;
    QString msgParameters;
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

    QString messageHeader = logic.mergeHeaders("../QWebService/headers/qwebmethod.h",
                                         "../QWebService/headers/qwebmethod_p.h");

    int beginIndex = 0;
    int endIndex = 0;
    // Replace QWebMethod with msgName.
    messageHeader.replace("QWebMethod", msgName, Qt::CaseSensitive);

    // Add constructor.
    if (msgParameters != QString()) {
        beginIndex = messageHeader.indexOf("~") - 4;
        messageHeader.insert(beginIndex,
                             flags->tab() + msgName + "(QObject *parent, "
                             + msgParameters + ");"
                             + flags->endLine());
    }

    // Replace parameters list for setParameters().
    beginIndex = messageHeader.indexOf("setParameters(") + 14;
    endIndex = messageHeader.indexOf(");", beginIndex);
    messageHeader.replace(beginIndex, endIndex - beginIndex, msgParameters);

    // Create asynchronous invokeMethod().
    if ((msgParameters != QString()) && !((flags->flags() & Flags::CompactMode)
                                   && (flags->flags() & Flags::Synchronous))) {
        beginIndex = messageHeader.indexOf("invokeMethod(");
        endIndex = messageHeader.indexOf(");", beginIndex) + 2;
        messageHeader.insert(endIndex,
                             flags->endLine() + flags->tab()
                             + "bool invokeMethod("
                             + msgParameters
                             + ");" + flags->endLine());
    }

    // Create synchronous static invokeMethod().
    if (!((flags->flags() & Flags::CompactMode)
          && (flags->flags() & Flags::Asynchronous)))
    {
        beginIndex = messageHeader.indexOf("invokeMethod(");
        endIndex = messageHeader.indexOf(");", beginIndex) + 2;
        if (msgParameters != QString()) {
            messageHeader.insert(endIndex,
                                 flags->tab()
                                 + "QString static invokeMethod(QObject *parent,"
                                 + msgParameters
                                 + ");" + flags->endLine());
        } else {
            messageHeader.insert(endIndex,
                                 flags->tab()
                                 + "QString static invokeMethod(QObject *parent"
                                 + ");" + flags->endLine());
        }
    }

    // Create parameters list in declarative form.
    beginIndex = messageHeader.indexOf("};", endIndex) - 5;
    messageHeader.insert(beginIndex,
                         flags->tab() + "// -------------------------" + flags->endLine()
                         + flags->tab() + "// Parameters:" + flags->endLine());
    QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();
    foreach (QString s, tempMap.keys()) {
        beginIndex = messageHeader.indexOf("};", endIndex) - 5;
        messageHeader.insert(beginIndex, flags->tab() + tempMap.value(s).typeName()
                             + " " + s + ";" + flags->endLine());
    }
    beginIndex = messageHeader.indexOf("};", endIndex) - 5;
    messageHeader.insert(beginIndex, flags->tab() + "// End of parameters" + flags->endLine()
                         + flags->tab() + "// -------------------------" + flags->endLine());

    // Begin writing:
    QTextStream out(&file);
//    out.setCodec("UTF-8");
    // Might break on curious names
    out << "#ifndef " << msgName.toUpper() << "_H" << endl;
    out << "#define " << msgName.toUpper() << "_H" << endl;
    out << endl;
    out << messageHeader;
    out << "#endif // " << msgName.toUpper() << "_H" << endl;
    // EOF (message header)

    file.close();
    return true;
}

/*!
    \internal
  */
bool MessageGenerator::createMessageSource(QWebServiceMethod *msg)
{
    QString msgName = msg->methodName();
    QFile file(QString(workingDir.path() + QLatin1String("/")
                       + msgName + QLatin1String(".cpp")));
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState(QLatin1String("Error: could not open message"
                                             "source file for writing."));

    // Possible problem in case of multi-return.
    QString msgReplyName = msg->returnValueName().first();

    QString msgReplyType;
    QString msgParameters;
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

    QString messageSource = logic.stripFile(
                logic.readFile("../QWebService/sources/qwebmethod.cpp"),
                logic.Source);

    int beginIndex = 0;
    int endIndex = 0;
    // Remove traces of QWebMethodPrivate.
    messageSource.remove("Q_D(QWebMethod);");
    messageSource.remove("Q_D(const QWebMethod);");
    messageSource.remove("Q_Q(QWebMethod);");
    messageSource.remove("Q_Q(const QWebMethod);");
    messageSource.remove("d->");
    messageSource.remove("q->");
    messageSource.remove(", d_ptr(new QWebMethodPrivate)");
    // Remove protected constructor.
    beginIndex = messageSource.indexOf("QWebMethod::QWebMethod(QWebMethodPrivate");
    endIndex = messageSource.indexOf("}", beginIndex) + 2;
    messageSource.remove(beginIndex, endIndex - beginIndex);
    messageSource.replace("QWebMethodPrivate", "QWebMethod");

    // Replace QWebMethod with msgName.
    messageSource.replace("QWebMethod", msgName, Qt::CaseSensitive);

    beginIndex = messageSource.indexOf("QObject(") + 15;
    beginIndex = messageSource.indexOf("set", beginIndex) - 5;
    endIndex = messageSource.indexOf("}", beginIndex);
    messageSource.remove(beginIndex, endIndex - beginIndex);
    { // Set flags as default params.
        QString tmp = flags->endLine() + flags->tab()
                + "setProtocol(" + flags->protocolString() + ");" + flags->endLine()
                + flags->tab() + "setHttpMethod("
                + flags->httpMethodString() + ");" + flags->endLine()
                + flags->tab() + "hostUrlUsed.setHost(\"";
        if (msg->host() != QString())
            tmp += msg->host();
        else
            tmp+= msg->targetNamespace();
        tmp += "\");" + flags->endLine();

        messageSource.insert(beginIndex, tmp);
    }

    // Add host info to all constructors.
    addCustomCodeToConstructor(messageSource, msg, beginIndex);
    addCustomCodeToConstructor(messageSource, msg, beginIndex);

    // Add specialised constructor.
    if (msgParameters != QString()) {
        beginIndex = messageSource.indexOf(msgName + "::~") - 2;
        QString tmp = msgName + "::" + msgName + "(QObject *parent, "
                + msgParameters + ") :" + flags->endLine()
                + flags->tab() + "QObject(parent)" + flags->endLine()
                + "{" + flags->endLine()
                + assignAllParameters(msg)
                + flags->tab() + "setProtocol(" + flags->protocolString()
                + ");" + flags->endLine()
                + flags->tab() + "setHttpMethod("
                + flags->httpMethodString() + ");" + flags->endLine()
                + flags->tab() + "hostUrlUsed.setHost(\"";
        if (msg->host() != QString())
            tmp += msg->host();
        else
            tmp+= msg->targetNamespace();
        tmp += "\");" + flags->endLine()
                + "}" + flags->endLine();

        messageSource.insert(beginIndex, tmp);
    }

    // Create setParameters() method.
    beginIndex = messageSource.indexOf("::setParameters(") + 16;
    endIndex = messageSource.indexOf("}", beginIndex) + 1;
    messageSource.remove(beginIndex, endIndex - beginIndex);
    messageSource.insert(beginIndex, msgParameters + ")" + flags->endLine()
                         + "{" + flags->endLine()
                         + assignAllParameters(msg)
                         + "}" + flags->endLine() + flags->endLine());

    // Create invokeMethod(), which uses all parameters of a web method.
    if ((msgParameters != QString())
            && !((flags->flags() & Flags::CompactMode)
                 && (flags->flags() & Flags::Synchronous))) {
        // Finding end of a method just after last invokeMethod.
        beginIndex = messageSource.indexOf("QString bookABand::replyRead()") - 2;
        messageSource.insert(beginIndex, "bool " + msgName + "::invokeMethod("
                             + msgParameters + ")" + flags->endLine()
                             + "(" + flags->endLine()
                             + assignAllParameters(msg)
                             + flags->tab() + "return invokeMethod();" + flags->endLine()
                             + "}" + flags->endLine());
    }

    // Create static invokeMethod().
    if (!((flags->flags() & Flags::CompactMode)
          && (flags->flags() & Flags::Asynchronous))) {
        // Finding end of a method just after last invokeMethod.
        beginIndex = messageSource.indexOf("QString bookABand::replyRead()") - 2;

        QString body = "QString " + msgName + "::invokeMethod(QObject *parent";
        if (msgParameters != QString()) {
            body += ", " + msgParameters;
        }
        body += ")" + flags->endLine() + "{" + flags->endLine()
                + flags->tab() + msgName + " qsm(parent);" + flags->endLine()
                + flags->tab() + "qsm.setParameters(";

        QString tempS;
        QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
            tempS += s + ", ";
        }
        tempS.chop(2);

        // Add waiting loop.
        body += tempS + ");" + flags->endLine()
                + flags->tab() + "qsm.sendMessage();" + flags->endLine()
                + flags->tab() + "forever {" + flags->endLine()
                + flags->tab() + flags->tab() + "if (qsm.replyReceived) {" + flags->endLine()
                + flags->tab() + flags->tab() + flags->tab() + "return qsm.reply;" + flags->endLine()
                + flags->tab() + flags->tab() + "} else {" + flags->endLine()
                + flags->tab() + flags->tab() + flags->tab() + "qApp->processEvents();" + flags->endLine()
                + flags->tab() + flags->tab() + "}"  +flags->endLine()
                + flags->tab() + "}" + flags->endLine()
                + "}" + flags->endLine();
        messageSource.insert(beginIndex, body);
    }

    // Add parameterNames() method.
    beginIndex = messageSource.indexOf("::parameterNames() const") + 24;
    endIndex = messageSource.indexOf("}", beginIndex);
    messageSource.remove(beginIndex, endIndex - beginIndex);
    { // Create new method body.
        QString tmp = flags->endLine() + "{" + flags->endLine()
                + flags->tab() + "QMap<QString, QVariant> parameters;";

        QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
            tmp += flags->tab() + "parameters.insert(\"" + s + "\", QVariant(";
            QString tmpName = tempMap.value(s).typeName();
            if (tmpName != QLatin1String("int"))
                tmp += tmpName;
            tmp += "(" + tempMap.value(s).toString() + ")));" + flags->endLine();
        }
        tmp += flags->tab() + "return (QStringList) parameters.keys();" + flags->endLine();
        messageSource.insert(beginIndex, tmp);
    }

    // Add parameterNamesTypes() method.
    beginIndex = messageSource.indexOf("::parameterNamesTypes() const") + 29;
    endIndex = messageSource.indexOf("}", beginIndex);
    messageSource.remove(beginIndex, endIndex - beginIndex);
    { // Create new method body.
        QString tmp = flags->endLine() + "{" + flags->endLine()
                + flags->tab() + "QMap<QString, QVariant> parameters;";

        QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
            tmp += flags->tab() + "parameters.insert(\"" + s + "\", QVariant(";
            QString tmpName = tempMap.value(s).typeName();
            if (tmpName != QLatin1String("int"))
                tmp += tmpName;
            tmp += "(" + tempMap.value(s).toString() + ")));" + flags->endLine();
        }
        tmp += flags->tab() + "return parameters;" + flags->endLine();
        messageSource.insert(beginIndex, tmp);
    }

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "#include \"";
    if (!(flags->flags() & Flags::AllInOneDirStructure))
        out << "../headers/";
    out << msgName << ".h\"" << endl;
    out << endl;
    out << messageSource;
    // EOF (message)
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
    QFile file(QString(workingDir.path() + QLatin1String("/main.cpp")));
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState(QLatin1String("Error: could not open Web "
                                             "Service header file for writing."));

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

/*!
  \internal

  Assigns all message parameters to this-><paramName>.
  */
QString MessageGenerator::assignAllParameters(QWebServiceMethod *msg)
{
    QString result;
    QMap<QString, QVariant> tempMap = msg->parameterNamesTypes();

    foreach (QString s, tempMap.keys()) {
        result += flags->tab() + "this->" + s + " = " + s + ";" + flags->endLine();
    }

    return result;
}

/*!
  \internal

  Inserts custom data to constructor. Used in create sources.
  */
void MessageGenerator::addCustomCodeToConstructor(QString &sourceCode,
                                                  QWebServiceMethod *msg,
                                                  int &beginIndex)
{
    beginIndex = sourceCode.indexOf("QObject(", beginIndex) + 15;
    beginIndex = sourceCode.indexOf("}", beginIndex) - 1;
    {
        QString tmp = flags->endLine()
                + flags->tab() + "hostUrlUsed.setHost(\"";
        if (msg->host() != QString())
            tmp += msg->host();
        else
            tmp+= msg->targetNamespace();
        tmp += "\");" + flags->endLine();

        sourceCode.insert(beginIndex, tmp);
    }
}
