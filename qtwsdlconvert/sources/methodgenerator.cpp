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

#include "../headers/methodgenerator.h"

/*!
    \class MethodGenerator
    \brief Creates code for web methods.

    Creates methods (both headers and sources) by generating the entire code
    or subclassing QWebServiceMethod (depending on whether --subclass flag is set).

    Generation is based on templates located in ./qtwsdlconverter/templates (for
    subclassed methods) and on real QWebMethod code (for non-subclassed methods).
  */

/*!
    Constructs QObject using \a parent, initialises MethodGenerator
    with methods (\a mthds), working directory (\a wrkDir), and flags (\a flgs).
  */
MethodGenerator::MethodGenerator(QMap<QString, QWebMethod *> *mthds,
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
QString MethodGenerator::errorMessage()
{
    return m_errorMessage;
}

/*!
    \internal
  */
bool MethodGenerator::enterErrorState(const QString errMessage)
{
    m_errorMessage += errMessage + QLatin1String("\n");
    return false;
}

/*!
    Creates web methods (both headers and sources) by generating the entire code
    or subclassing QWebServiceMethod (depending on whether --subclass flag is set).

    Returns true if successful.
  */
bool MethodGenerator::createMethods()
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
        QWebMethod *m = methods->value(s);

        if (flags->flags() & Flags::Subclass) {
            if (!createSubclassedMethodHeader(m))
                return enterErrorState("Creating header for web method \""
                                       + m->methodName() + "\" failed!");
        } else {
            if (!createMethodHeader(m))
                return enterErrorState("Creating header for web method \""
                                       + m->methodName() + "\" failed!");
        }
    }

    if (!(flags->flags() & Flags::AllInOneDirStructure)) {
        workingDir.cdUp();
        workingDir.cd(QLatin1String("sources"));
    }

    foreach (QString s, methods->keys()) {
        QWebMethod *n = methods->value(s);

        if (flags->flags() & Flags::Subclass) {
            if (!createSubclassedMethodSource(n))
                return enterErrorState("Creating header for web method \""
                                       + n->methodName() + "\" failed!");
        } else {
            if (!createMethodSource(n))
                return enterErrorState("Creating source for web method \""
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
bool MethodGenerator::createSubclassedMethodHeader(QWebMethod *mtd)
{
    QString mtdName = mtd->methodName();
    QFile file(QString(workingDir.path() + QLatin1String("/")
                       + mtdName + QLatin1String(".h")));
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState(QLatin1String("Error: could not open web method "
                                             "header file for writing."));

    // Possible problem in case of multi-return.
    QString mtdReplyName = mtd->returnValueName().first();

    QString mtdReplyType;
    QString mtdParameters;
    {
        // Create mtdReplyType
        QMap<QString, QVariant> tempMap = mtd->returnValueNameType();

        foreach (QString s, tempMap.keys()) {
            mtdReplyType += tempMap.value(s).typeName();
            break;
        }

        tempMap.clear();
        tempMap = mtd->parameterNamesTypes();

        // Create mtdParameters (comma separated list)
        foreach (QString s, tempMap.keys()) {
            mtdParameters += QString(tempMap.value(s).typeName())
                    + " " + s + ", ";
        }
        mtdParameters.chop(2);
    }

    QString methodHeader = logic.readFile("../qtWsdlConverter/templates/method_header");
    int beginIndex = 0;

    methodHeader.replace("%headerIfndef%", mtdName.toUpper() + "_H");
    methodHeader.replace("%method%", mtdName);
    methodHeader.replace("%params%", mtdParameters);

    // Create custom constructor.
    beginIndex = logic.removeTag(methodHeader, "%methodConstructor%");
    if (mtdParameters != QString()) {
        QString toInsert = flags->tab() + mtdName + "(" + mtdParameters
                + ", QObject *parent = 0);" + flags->tab();
        methodHeader.insert(beginIndex, toInsert);
        beginIndex += toInsert.length();
    }

    beginIndex = logic.removeTag(methodHeader, "%methodInvoke%");
    if (mtdParameters != QString()
            && !(flags->flags() & Flags::CompactMode)
            && (flags->flags() & Flags::Asynchronous)) {
        QString toInsert = "bool invokeMethod(" + mtdParameters
                + ");" + flags->endLine();
        methodHeader.insert(beginIndex, toInsert);
    }

    beginIndex = logic.removeTag(methodHeader, "%staticInvoke%");
    if (!(flags->flags() & Flags::CompactMode)
          && (flags->flags() & Flags::Synchronous)) {
        QString toInsert = "QString static invokeMethod(QObject *parent";
        if (mtdParameters != QString())
            toInsert += ", " + mtdParameters;
        toInsert += ");" + flags->endLine();
        methodHeader.insert(beginIndex, toInsert);
    }

    { // Create parameters list in declarative form.
        beginIndex = logic.removeTag(methodHeader, "%paramsDeclaration%");

        QString toInsert = flags->tab() + "// -------------------------"
                + flags->endLine() + flags->tab()
                + "// Parameters:" +flags->endLine();

        QMap<QString, QVariant> tempMap = mtd->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
            toInsert += flags->tab() + tempMap.value(s).typeName()
                    + " " + s + ";" + flags->endLine();
        }
       toInsert += flags->tab() + "// End of parameters." + flags->endLine()
               + flags->tab() + "// -------------------------" + flags->endLine();

        methodHeader.insert(beginIndex, toInsert);
        beginIndex += toInsert.length();
    }

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << methodHeader;
    // EOF (SOAP method)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal

    Creates a subclassed web method source file.
  */
bool MethodGenerator::createSubclassedMethodSource(QWebMethod *mtd)
{
    QString mtdName = mtd->methodName();
    QFile file(QString(workingDir.path() + QLatin1String("/")
                       + mtdName + QLatin1String(".cpp")));
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState(QLatin1String("Error: could not open web method "
                                             "source file for writing."));

    // Possible problem in case of multi-return.
    QString mtdReplyName = mtd->returnValueName().first();

    QString mtdReplyType;
    QString mtdParameters;
    {
        QMap<QString, QVariant> tempMap = mtd->returnValueNameType();

        foreach (QString s, tempMap.keys()) {
            mtdReplyType += tempMap.value(s).typeName();
            break;
        }

        tempMap.clear();
        tempMap = mtd->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
            mtdParameters += QString(tempMap.value(s).typeName())
                    + " " + s + ", ";
        }
        mtdParameters.chop(2);
    }

    QString methodSource = logic.readFile("../qtWsdlConverter/templates/method_source");
    int beginIndex = 0;

    methodSource.replace("%method%", mtdName);
    methodSource.replace("%params%", mtdParameters);
    methodSource.replace("%assignParams%", assignAllParameters(mtd));

    beginIndex = logic.removeTag(methodSource, "%include%");
    QString toInsert = "#include \"";
    if (!(flags->flags() & Flags::AllInOneDirStructure))
        toInsert += "../headers/";
    toInsert += mtdName + ".h\"";
    methodSource.insert(beginIndex, toInsert);

    beginIndex = logic.removeTag(methodSource, "%methodConstructor%");
    if (mtdParameters != QString()) {
        toInsert = mtdName + "::" + mtdName + "(" + mtdParameters
                + ", QObject *parent) :" + flags->endLine()
                + flags->tab() + "QWebServiceMethod(parent)" +  flags->endLine()
                + "{" +  flags->endLine()
                + assignAllParameters(mtd) +  flags->tab()
                + "configure();" +  flags->endLine()
                + "}";

        methodSource.insert(beginIndex, toInsert);
    }

    beginIndex = logic.removeTag(methodSource, "%methodInvoke%");
    if (mtdParameters != QString()
            && !(flags->flags() & Flags::CompactMode)
            && (flags->flags() & Flags::Asynchronous)) {
        QString toInsert = "bool " + mtdName + "::invokeMethod(" + mtdParameters
                + ")" + flags->endLine()
                + "{" + flags->endLine()
                + assignAllParameters(mtd) + flags->tab()
                + "return invokeMethod();" + flags->endLine()
                + "}" + flags->endLine();
        methodSource.insert(beginIndex, toInsert);
    }

    beginIndex = logic.removeTag(methodSource, "%staticInvoke%");
    if (!(flags->flags() & Flags::CompactMode)
          && (flags->flags() & Flags::Synchronous)) {
        toInsert = "/* STATIC */" + flags->endLine()
                + "QString " + mtdName + "::invokeMethod(QObject *parent";
        if (mtdParameters != QString())
            toInsert += ", " + mtdParameters;
        toInsert += ")" + flags->endLine()
                + "{" + flags->endLine()
                + flags->tab() + mtdName + " qsm(";
        { // Assign all parameters.
            QString tempS;
            QMap<QString, QVariant> tempMap = mtd->parameterNamesTypes();

            foreach (QString s, tempMap.keys()) {
                tempS += s + ", ";
            }
//            tempS.chop(2);
            toInsert += tempS + "parent);" + flags->endLine();
        }
        toInsert += flags->endLine() + flags->tab()
                + "qsm.invokeMethod();" + flags->endLine()
                + flags->tab() + "// TODO: ADD ERROR HANDLING!" + flags->endLine()
                + flags->tab() + "forever" + flags->endLine()
                + flags->tab() + "{" + flags->endLine()
                + flags->tab() + flags->tab() + "if (qsm.isReplyReady())" + flags->endLine()
                + flags->tab() + flags->tab() + flags->tab() + "return qsm.replyRead();"
                + flags->endLine()
                + flags->tab() + flags->tab() + "else" + flags->endLine()
                + flags->tab() + flags->tab() + "{" + flags->endLine()
                + flags->tab() + flags->tab() + flags->tab()
                + "QCoreApplication::instance()->processEvents();"
                + flags->endLine()
                + flags->tab() + flags->tab() + "}" + flags->endLine()
                + flags->tab() + "}" + flags->endLine()
                + "}";

        methodSource.insert(beginIndex, toInsert);
    }
    beginIndex = logic.removeTag(methodSource, "%configure%");
    toInsert = flags->tab() + "setHost(\"" + mtd->host() + "\");"
            + flags->endLine()
            + flags->tab() + "setProtocol(" + flags->protocolString(false) + ");"
            + flags->endLine()
            + flags->tab() + "setHttpMethod(" + flags->httpMethodString() + ");"
            + flags->endLine()
            + flags->tab() + "setMethodName(\"" + mtd->methodName() + "\");"
            + flags->endLine()
            + flags->tab() + "setTargetNamespace(\"" + mtd->targetNamespace() + "\");";
    methodSource.insert(beginIndex, toInsert);

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << methodSource;
    // EOF (SOAP method)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal

    Creates a non-subclassed method header.
  */
bool MethodGenerator::createMethodHeader(QWebMethod *mtd)
{
    QString mtdName = mtd->methodName();
    QFile file(QString(workingDir.path() + QLatin1String("/")
                       + mtdName + QLatin1String(".h")));
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState(QLatin1String("Error: could not open web method "
                                             "header file for writing."));

    // Possible problem in case of multi-return.
    QString mtdReplyName = mtd->returnValueName().first();

    QString mtdReplyType;
    QString mtdParameters;
    {
        // Create mtdReplyType
        QMap<QString, QVariant> tempMap = mtd->returnValueNameType();

        foreach (QString s, tempMap.keys()) {
            mtdReplyType += tempMap.value(s).typeName();
            break;
        }

        tempMap.clear();
        tempMap = mtd->parameterNamesTypes();

        // Create mtdParameters (comma separated list)
        foreach (QString s, tempMap.keys()) {
            mtdParameters += QString(tempMap.value(s).typeName()) + " " + s + ", ";
        }
        mtdParameters.chop(2);
    }

    QString methodHeader = logic.mergeHeaders("../QWebService/headers/qwebmethod.h",
                                         "../QWebService/headers/qwebmethod_p.h");

    int beginIndex = 0;
    int endIndex = 0;
    // Replace QWebMethod with mtdName.
    methodHeader.replace("QWebMethod", mtdName, Qt::CaseSensitive);

    // Add constructor.
    if (mtdParameters != QString()) {
        beginIndex = methodHeader.indexOf("~") - 4;
        methodHeader.insert(beginIndex,
                             flags->tab() + mtdName + "(QObject *parent, "
                             + mtdParameters + ");"
                             + flags->endLine());
    }

    // Replace parameters list for setParameters().
    beginIndex = methodHeader.indexOf("setParameters(") + 14;
    endIndex = methodHeader.indexOf(");", beginIndex);
    methodHeader.replace(beginIndex, endIndex - beginIndex, mtdParameters);

    // Create asynchronous invokeMethod().
    if ((mtdParameters != QString()) && !(flags->flags() & Flags::CompactMode)
                                   && (flags->flags() & Flags::Asynchronous)) {
        beginIndex = methodHeader.indexOf("invokeMethod(");
        endIndex = methodHeader.indexOf(");", beginIndex) + 2;
        methodHeader.insert(endIndex,
                             flags->endLine() + flags->tab()
                             + "bool invokeMethod("
                             + mtdParameters
                             + ");" + flags->endLine());
    }

    // Create synchronous static invokeMethod().
    if (!(flags->flags() & Flags::CompactMode)
            && (flags->flags() & Flags::Synchronous))
    {
        beginIndex = methodHeader.indexOf("invokeMethod(");
        endIndex = methodHeader.indexOf(");", beginIndex) + 2;
        if (mtdParameters != QString()) {
            methodHeader.insert(endIndex,
                                 flags->tab()
                                 + "QString static invokeMethod(QObject *parent, "
                                 + mtdParameters
                                 + ");" + flags->endLine());
        } else {
            methodHeader.insert(endIndex,
                                 flags->tab()
                                 + "QString static invokeMethod(QObject *parent"
                                 + ");" + flags->endLine());
        }
    }

    // Create parameters list in declarative form.
    beginIndex = methodHeader.indexOf("};", endIndex) - 5;
    methodHeader.insert(beginIndex,
                         flags->tab() + "// -------------------------" + flags->endLine()
                         + flags->tab() + "// Parameters:" + flags->endLine());
    QMap<QString, QVariant> tempMap = mtd->parameterNamesTypes();
    foreach (QString s, tempMap.keys()) {
        beginIndex = methodHeader.indexOf("};", endIndex) - 5;
        methodHeader.insert(beginIndex, flags->tab() + tempMap.value(s).typeName()
                             + " " + s + ";" + flags->endLine());
    }
    beginIndex = methodHeader.indexOf("};", endIndex) - 5;
    methodHeader.insert(beginIndex, flags->tab() + "// End of parameters" + flags->endLine()
                         + flags->tab() + "// -------------------------" + flags->endLine());

    // Begin writing:
    QTextStream out(&file);
//    out.setCodec("UTF-8");
    // Might break on curious names
    out << "#ifndef " << mtdName.toUpper() << "_H" << flags->endLine();
    out << "#define " << mtdName.toUpper() << "_H" << flags->endLine();
    out << flags->endLine();
    out << methodHeader;
    out << "#endif // " << mtdName.toUpper() << "_H" << flags->endLine();
    // EOF (method header)

    file.close();
    return true;
}

/*!
    \internal

    Creates a non-subclassed method source.
  */
bool MethodGenerator::createMethodSource(QWebMethod *mtd)
{
    QString mtdName = mtd->methodName();
    QFile file(QString(workingDir.path() + QLatin1String("/")
                       + mtdName + QLatin1String(".cpp")));
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState(QLatin1String("Error: could not open web method"
                                             "source file for writing."));

    // Possible problem in case of multi-return.
    QString mtdReplyName = mtd->returnValueName().first();

    QString mtdReplyType;
    QString mtdParameters;
    {
        QMap<QString, QVariant> tempMap = mtd->returnValueNameType();

        foreach (QString s, tempMap.keys()) {
            mtdReplyType += tempMap.value(s).typeName();
            break;
        }

        tempMap.clear();
        tempMap = mtd->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
            mtdParameters += QString(tempMap.value(s).typeName())
                    + " " + s + ", ";
        }
        mtdParameters.chop(2);
    }

    QString methodSource = logic.stripFile(
                logic.readFile("../QWebService/sources/qwebmethod.cpp"),
                logic.Source);

    int beginIndex = 0;
    int endIndex = 0;
    // Remove traces of QWebMethodPrivate.
    methodSource.remove("Q_D(QWebMethod);");
    methodSource.remove("Q_D(const QWebMethod);");
    methodSource.remove("Q_Q(QWebMethod);");
    methodSource.remove("Q_Q(const QWebMethod);");
    methodSource.remove("d->");
    methodSource.remove("q->");
    methodSource.remove(", d_ptr(new QWebMethodPrivate)");
    // Remove protected constructor.
    beginIndex = methodSource.indexOf("QWebMethod::QWebMethod(QWebMethodPrivate");
    endIndex = methodSource.indexOf("}", beginIndex) + 2;
    methodSource.remove(beginIndex, endIndex - beginIndex);
    methodSource.replace("QWebMethodPrivate", "QWebMethod");

    // Replace QWebMethod with mtdName.
    methodSource.replace("QWebMethod", mtdName, Qt::CaseSensitive);

    // Add host info to all constructors.
    addCustomCodeToConstructor(methodSource, mtd, beginIndex);
    addCustomCodeToConstructor(methodSource, mtd, beginIndex);

    // Add specialised constructor.
    if (mtdParameters != QString()) {
        beginIndex = methodSource.indexOf(mtdName + "::~") - 2;
        QString tmp = mtdName + "::" + mtdName + "(QObject *parent, "
                + mtdParameters + ") :" + flags->endLine()
                + flags->tab() + "QObject(parent)" + flags->endLine()
                + "{" + flags->endLine()
                + assignAllParameters(mtd)
                + flags->tab() + "setProtocol(" + flags->protocolString()
                + ");" + flags->endLine()
                + flags->tab() + "setHttpMethod("
                + flags->httpMethodString() + ");" + flags->endLine()
                + flags->tab() + "m_hostUrl.setHost(\"";
        if (mtd->host() != QString())
            tmp += mtd->host();
        else
            tmp+= mtd->targetNamespace();
        tmp += "\");" + flags->endLine()
                + "}" + flags->endLine();

        methodSource.insert(beginIndex, tmp);
    }

    // Create setParameters() method.
    beginIndex = methodSource.indexOf("::setParameters(") + 16;
    endIndex = methodSource.indexOf("}", beginIndex) + 1;
    methodSource.remove(beginIndex, endIndex - beginIndex);
    methodSource.insert(beginIndex, mtdParameters + ")" + flags->endLine()
                         + "{" + flags->endLine()
                         + assignAllParameters(mtd)
                         + "}" + flags->endLine() + flags->endLine());

    // Create asynchronous invokeMethod(), which uses all parameters of a web method.
    if ((mtdParameters != QString())
            && !(flags->flags() & Flags::CompactMode)
            && (flags->flags() & Flags::Asynchronous)) {
        // Finding end of a method just after last invokeMethod.
        beginIndex = methodSource.indexOf("QString " + mtdName + "::replyRead()") - 2;
        methodSource.insert(beginIndex, flags->endLine()
                            + "bool " + mtdName + "::invokeMethod("
                            + mtdParameters + ")" + flags->endLine()
                            + "{" + flags->endLine()
                            + assignAllParameters(mtd)
                            + flags->tab() + "return invokeMethod();" + flags->endLine()
                            + "}" + flags->endLine());
    }

    // Create synchronous, static invokeMethod().
    if (!(flags->flags() & Flags::CompactMode)
            && (flags->flags() & Flags::Synchronous)) {
        // Finding end of a method just after last invokeMethod.
        beginIndex = methodSource.indexOf("QString " + mtdName + "::replyRead()") - 2;

        QString body = "QString " + mtdName + "::invokeMethod(QObject *parent";
        if (mtdParameters != QString()) {
            body += ", " + mtdParameters;
        }
        body += ")" + flags->endLine() + "{" + flags->endLine()
                + flags->tab() + mtdName + " qsm(parent);" + flags->endLine()
                + flags->tab() + "qsm.setParameters(";

        QString tempS;
        QMap<QString, QVariant> tempMap = mtd->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
            tempS += s + ", ";
        }
        tempS.chop(2);

        // Add waiting loop.
        body += tempS + ");" + flags->endLine()
                + flags->tab() + "qsm.invokeMethod();" + flags->endLine()
                + flags->tab() + "forever {" + flags->endLine()
                + flags->tab() + flags->tab() + "if (qsm.isReplyReady()) {" + flags->endLine()
                + flags->tab() + flags->tab() + flags->tab() + "return qsm.replyRead();" + flags->endLine()
                + flags->tab() + flags->tab() + "} else {" + flags->endLine()
                + flags->tab() + flags->tab() + flags->tab()
                + "QCoreApplication::instance()->processEvents();" + flags->endLine()
                + flags->tab() + flags->tab() + "}"  +flags->endLine()
                + flags->tab() + "}" + flags->endLine()
                + "}" + flags->endLine();
        methodSource.insert(beginIndex, flags->endLine() + body);
    }

    // Add parameterNames() method.
    beginIndex = methodSource.indexOf("::parameterNames() const") + 24;
    endIndex = methodSource.indexOf("}", beginIndex);
    methodSource.remove(beginIndex, endIndex - beginIndex);
    { // Create new method body.
        QString tmp = flags->endLine() + "{" + flags->endLine()
                + flags->tab() + "QMap<QString, QVariant> parameters;";

        QMap<QString, QVariant> tempMap = mtd->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
            tmp += flags->tab() + "parameters.insert(\"" + s + "\", QVariant(";
            QString tmpName = tempMap.value(s).typeName();
            if (tmpName != QLatin1String("int"))
                tmp += tmpName;
            tmp += "(" + tempMap.value(s).toString() + ")));" + flags->endLine();
        }
        tmp += flags->tab() + "return (QStringList) parameters.keys();" + flags->endLine();
        methodSource.insert(beginIndex, tmp);
    }

    // Add parameterNamesTypes() method.
    beginIndex = methodSource.indexOf("::parameterNamesTypes() const") + 29;
    endIndex = methodSource.indexOf("}", beginIndex);
    methodSource.remove(beginIndex, endIndex - beginIndex);
    { // Create new method body.
        QString tmp = flags->endLine() + "{" + flags->endLine()
                + flags->tab() + "QMap<QString, QVariant> parameters;";

        QMap<QString, QVariant> tempMap = mtd->parameterNamesTypes();

        foreach (QString s, tempMap.keys()) {
            tmp += flags->tab() + "parameters.insert(\"" + s + "\", QVariant(";
            QString tmpName = tempMap.value(s).typeName();
            if (tmpName != QLatin1String("int"))
                tmp += tmpName;
            tmp += "(" + tempMap.value(s).toString() + ")));" + flags->endLine();
        }
        tmp += flags->tab() + "return parameters;" + flags->endLine();
        methodSource.insert(beginIndex, tmp);
    }

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "#include \"";
    if (!(flags->flags() & Flags::AllInOneDirStructure))
        out << "../headers/";
    out << mtdName << ".h\"" << flags->endLine();
    out << flags->endLine();
    out << methodSource;
    // EOF (method source)
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
bool MethodGenerator::createMainCpp()
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
        << "remember to remove it from .pro file, too). */" << flags->endLine();
    out << "#include \"../headers/band_ws.h\"" << flags->endLine();
    out << "int main() {return 0;}" << flags->endLine();
    // EOF (main.cpp)
    // ---------------------------------

    file.close();
    return true;
}

/*!
  \internal

  Assigns all method parameters to this-><paramName>.
  */
void MethodGenerator::assignAllParameters(QWebMethod *mtd, QTextStream &out)
{
    QMap<QString, QVariant> tempMap = mtd->parameterNamesTypes();

    foreach (QString s, tempMap.keys()) {
        out << flags->tab() + "this->" << s << " = " << s << ";" << flags->endLine();
    }
}

/*!
  \internal

  Assigns all method parameters to this-><paramName>.
  */
QString MethodGenerator::assignAllParameters(QWebMethod *mtd)
{
    QString result;
    QMap<QString, QVariant> tempMap = mtd->parameterNamesTypes();

    foreach (QString s, tempMap.keys()) {
        result += flags->tab() + "this->" + s + " = " + s + ";" + flags->endLine();
    }

    return result;
}

/*!
  \internal

  Inserts custom data to constructor. Used in create sources.
  */
void MethodGenerator::addCustomCodeToConstructor(QString &sourceCode,
                                                  QWebMethod *mtd,
                                                  int &beginIndex)
{
    beginIndex = sourceCode.indexOf("QObject(", beginIndex) + 15;
    beginIndex = sourceCode.indexOf("}", beginIndex) - 1;
    {
        QString tmp = flags->endLine()
                + flags->tab() + "m_hostUrl.setHost(\"";
        if (mtd->host() != QString())
            tmp += mtd->host();
        else
            tmp+= mtd->targetNamespace();
        tmp += "\");" + flags->endLine();

        sourceCode.insert(beginIndex, tmp);
    }
}
