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

#include "../headers/codegenerator.h"

/*!
    \class CodeGenerator
    \brief Creates code for communicating with web services.

    Generates code, based on WSDL information and user defined flags.
    This class does not generate web methods' code itself - it is delegated
    to MethodGenerator class.

    Note:
    Generator also creates a dummy main.cpp file. It's needed only for
    successful compilation of freshly generated code. It is NOT NEEDED
    for any other reason. You can safely delete it in your project.
  */

/*!
    Constructs QObject using \a parent, initialises CodeGenerator.
  */
CodeGenerator::CodeGenerator(QObject *parent) :
    QObject(parent)
{
    errorState = false;
}

/*!
    \fn CodeGenerator::errorEncountered(const QString &errMessage)

    Singal emitted when CodeGenerator encounters an error. Carries \a errMessage
    for convenience.
  */

/*!
    Returns true if object is in error state.
  */
bool CodeGenerator::isErrorState()
{
    return errorState;
}

/*!
    \internal
  */
bool CodeGenerator::enterErrorState(const QString &errMessage)
{
    errorState = true;
    errorMessage += errMessage + QLatin1String("\n");
    emit errorEncountered(errMessage);
    return false;
}

/*!
    \internal
  */
void CodeGenerator::prepare()
{
    if (!(flags->flags() & Flags::AllInOneDirStructure)) {
        workingDir.mkdir(QLatin1String("headers"));
        workingDir.mkdir(QLatin1String("sources"));
    }

    methods = wsdl->methods();
}

/*!
    Performs the conversion in CodeGenerator. Data from WSDL (\a wsdl) is
    combined with options specified in flags (\a flags), and base class name
    (\a baseClassName) to create a complete set of classes in given directory
    (\a workingDir). For Qt reasons, \a parent is also needed, although
    it defaults to 0.

    Returns true if successful.
  */
bool CodeGenerator::create(QWsdl *wsdl, const QDir &workingDir, Flags *flags,
                           const QString &baseClassName, QObject *parent)
{
    CodeGenerator obj(parent);
    obj.baseClassName = baseClassName;
    obj.flags = flags;
    obj.workingDir = workingDir;
    obj.wsdl = wsdl;
    obj.logic = TemplateLogic(obj.flags);
    obj.prepare();

    if (!obj.createWebMethods())
        return false;
    if (!obj.createService())
        return false;
    if (!obj.createBuildSystemFile())
        return false;
    return true;
}

/*!
    \internal

    Manages creation of web service main files - they keep info
    about all web methods, host addresses etc.
  */
bool CodeGenerator::createService()
{
    if (!(flags->flags() & Flags::AllInOneDirStructure))
        workingDir.cd(QLatin1String("headers"));
    if (!createServiceHeader())
        return enterErrorState("Creating header for Web Service \""
                               + wsdl->webServiceName() + "\" failed!");

    if (!(flags->flags() & Flags::AllInOneDirStructure)) {
        workingDir.cdUp();
        workingDir.cd("sources");
    }

    if (!createServiceSource())
        return enterErrorState("Creating source for Web Service \""
                               + wsdl->webServiceName() + "\" failed!");

    if (!(flags->flags() & Flags::AllInOneDirStructure))
        workingDir.cdUp();
    return true;
}

/*!
    \internal

    Generates header of the web service file, based on template located in
    ./qtwsdlconverter/templates/service_header
  */
bool CodeGenerator::createServiceHeader()
{
    QString wsName;
    QMap<QString, QWebMethod *> *tempMap = wsdl->methods();

    if (baseClassName != QString())
        wsName = baseClassName;
    else
        wsName = wsdl->webServiceName();

    QFile file(QString(workingDir.path() + QLatin1String("/")
                       + wsName + QLatin1String(".h")));
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState(QLatin1String("Error: could not open Web Service"
                                             "header file for writing."));

    QString serviceHeader = logic.readFile("../qtWsdlConverter/templates/service_header");
    int beginIndex = 0;

    serviceHeader.replace("%headerIfndef%", wsName.toUpper() + "_H");
    serviceHeader.replace("%service%", wsName);

    // Includes.
    if (!(flags->flags() & Flags::NoMessagesStructure)) {
        // Include all methods.
        QStringList tempMp = wsdl->methodNames();
        beginIndex = logic.removeTag(serviceHeader, "%include%");

        foreach (QString s, tempMp) {
            QString toInsert = "#include \"" + s + ".h\"" + flags->endLine();
            serviceHeader.insert(beginIndex, toInsert);
            beginIndex += toInsert.length();
        }
    } else
    {
        serviceHeader.replace("%include%", "#include <QWebService>");
    }

    { // Declare all methods (as wrappers for method classes).
        beginIndex = logic.removeTag(serviceHeader, "%methodSend%");
        foreach (QString s, tempMap->keys()) {
            QString tmpReturn, tmpP;
            QWebMethod *m = tempMap->value(s);

            foreach (QString ret, m->returnValueNameType().keys()) {
                tmpReturn = m->returnValueNameType().value(ret).typeName();
                break; // This does not support multiple return values!
            }

            QMap<QString, QVariant> tempParam = m->parameterNamesTypes();
            // Create mtdParameters (comma separated list)
            foreach (QString param, tempParam.keys()) {
                tmpP += QString(tempParam.value(param).typeName()) + " "
                        + param + ", ";
            }
            tmpP.chop(2);

            QString toInsert;
            // Temporarily, all methods will return QString!
            if (flags->flags() & Flags::Synchronous)
                toInsert = flags->tab() + QLatin1String("QString ");
            else
                toInsert = flags->tab() + QLatin1String("void ");
            toInsert += s + flags->methodSuffix() + "(" + tmpP + ");" + flags->endLine();
            serviceHeader.insert(beginIndex, toInsert);
            beginIndex += toInsert.length();
        }
    }

    beginIndex = logic.removeTag(serviceHeader, "%methodReplyRead%");
    if (flags->flags() & Flags::Asynchronous) {
        // Declare getters of methods' replies.
        QString toInsert = flags->tab() + "// Method reply getters: " + flags->endLine();
        serviceHeader.insert(beginIndex, toInsert);
        beginIndex += toInsert.length();

        foreach (QString s, tempMap->keys()) {
            /* Code not ready for compact mode checking.
            if (flags->flags() & Flags::compactMode)
            {
                ; // Code compact mode here :)
            }
            else if (flags->flags() & Flags::fullMode || flags->flags() & Flags::debugMode)
            */
            {
                QString tmpReturn;
                QWebMethod *m = tempMap->value(s);
                foreach (QString ret, m->returnValueNameType().keys()) {
                    tmpReturn = m->returnValueNameType().value(ret).typeName();
                    break; // This does not support multiple return values!
                }
                toInsert = flags->tab() + tmpReturn + " " + s
                        + "ReplyRead();" + flags->endLine();
                serviceHeader.insert(beginIndex, toInsert);
                beginIndex += toInsert.length();
            }
        }
    }

    beginIndex = logic.removeTag(serviceHeader, "%methodReply%");
    if (flags->flags() & Flags::Asynchronous) {
        QString toInsert = "protected slots:" + flags->endLine();
        serviceHeader.insert(beginIndex, toInsert);
        beginIndex += toInsert.length();

        // Declare methods for processing asynchronous replies.
        foreach (QString s, tempMap->keys()) {
            /* Code not ready for compact mode checking.
            if (flags->flags() & Flags::compactMode)
            {
                ; // Code compact mode here :)
            }
            else if (flags->flags() & Flags::fullMode || flags->flags() & Flags::debugMode)
            */
            {
                toInsert = flags->tab() + "void " + s + "Reply(QString result);"
                        + flags->endLine();
                serviceHeader.insert(beginIndex, toInsert);
                beginIndex += toInsert.length();
            }
        }
    }

    beginIndex = logic.removeTag(serviceHeader, "%methodResult%");
    logic.removeTag(serviceHeader, "%methodMsg%");
    if (flags->flags() & Flags::Asynchronous) {
        // Declare reply variables for asynchronous mode.
        QString toInsert = flags->tab() + "// Method replies:" + flags->endLine();
        serviceHeader.insert(beginIndex, toInsert);
        beginIndex += toInsert.length();

        foreach (QString s, tempMap->keys()) {
            QString tmpReturn;
            QWebMethod *m = tempMap->value(s);

            foreach (QString ret, m->returnValueNameType().keys()) {
                tmpReturn = m->returnValueNameType().value(ret).typeName();
                break; // This does not support multiple return values!
            }
            toInsert = flags->tab() + tmpReturn + " " + s + "Result;"
                    + flags->endLine();
            serviceHeader.insert(beginIndex, toInsert);
            beginIndex += toInsert.length();
        }

        toInsert = flags->tab() + "// Methods:" + flags->endLine();
        foreach (QString s, tempMap->keys()) {
            if (!(flags->flags() & Flags::NoMessagesStructure))
                toInsert = flags->tab() + s + " ";
            else // sierdzio I don't particurarly like this implementation, will rethink it later.
                toInsert = flags->tab() + "QWebServiceMethod ";

            toInsert += s.toLower() + flags->objectSuffix() + ";" + flags->endLine();
            serviceHeader.insert(beginIndex, toInsert);
            beginIndex += toInsert.length();
        }
    }

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << serviceHeader;
    // EOF (Web Service header)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal

    Creates web service main file's source, based on tamplate located in
    ./qtWsdlConverter/templates/service_source
  */
bool CodeGenerator::createServiceSource()
{
    QString wsName;
    QMap<QString, QWebMethod *> *tempMap = wsdl->methods();
    if (baseClassName != QString())
        wsName = baseClassName;
    else
        wsName = wsdl->webServiceName();

    QFile file(QString(workingDir.path() + QLatin1String("/")
                       + wsName + QLatin1String(".cpp")));
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState(QLatin1String("Error: could not open Web Service"
                                             "source file for writing."));

    QString serviceSource = logic.readFile("../qtWsdlConverter/templates/service_source");
    int beginIndex = 0;

    serviceSource.replace("%service%", wsName);

    QString toInsert = "#include \"";
    if (!(flags->flags() & Flags::AllInOneDirStructure))
        toInsert += "../headers/";
    toInsert += wsName + ".h\"";
    serviceSource.replace("%include%", toInsert);

    beginIndex = logic.removeTag(serviceSource, "%connect%");
    if (flags->flags() & Flags::Asynchronous) {
        // Connect signals and slots for asynchronous mode.
        foreach (QString s, tempMap->keys()) {
            toInsert = flags->tab() + "connect(&" + s.toLower() + flags->objectSuffix()
                    + ", SIGNAL(replyReady(QString)), this, SLOT(";
            /* Code not ready for compact mode checking.
            if (flags->flags() & Flags::compactMode)
            {
                ; // Code compact mode here :)
            }
            else if (flags->flags() & Flags::fullMode || flags->flags() & Flags::debugMode)
            */
            {
                toInsert += s + "Reply(QString)";
            }
            toInsert += "));" + flags->endLine();

            serviceSource.insert(beginIndex, toInsert);
            beginIndex += toInsert.length();
        }
    }

    // Add constructor setters:
    beginIndex = logic.removeTag(serviceSource, "%constructorSetters%");
    toInsert = flags->tab() + "hostUrl.setHost(\"" + wsdl->host() + "\");" + flags->endLine()
            + flags->tab() + "errorState = false;" + flags->endLine()
            + flags->tab() + "isErrorState();";
    serviceSource.insert(beginIndex, toInsert);
    beginIndex += toInsert.length();

    // Create and return the QStringList containing method names:
    beginIndex = logic.removeTag(serviceSource, "%methodNames%");
    foreach (QString s, tempMap->keys()) {
        QWebMethod *m = tempMap->value(s);
        toInsert = flags->tab() + "result.append(\"" + m->methodName()
                + "\");" + flags->endLine();
        serviceSource.insert(beginIndex, toInsert);
        beginIndex += toInsert.length();
    };

    beginIndex = logic.removeTag(serviceSource, "%methodSend%");
    { // Define all methods (as wrappers for method classes).
        foreach (QString s, tempMap->keys()) {
            QString tmpReturn, tmpP, tmpPN;
            QWebMethod *m = tempMap->value(s);

            foreach (QString ret, m->returnValueNameType().keys()) {
                tmpReturn = m->returnValueNameType().value(ret).typeName();
                break; // This does not support multiple return values!
            }

            QMap<QString, QVariant> tempParam = m->parameterNamesTypes();

            // Create mtdParameters (comma separated list)
            foreach (QString param, tempParam.keys()) {
                tmpP += QLatin1String(tempParam.value(param).typeName())
                        + QLatin1String(" ")
                        + param
                        + QLatin1String(", ");
                tmpPN += QString(param + ", ");
            }
            tmpP.chop(2);
            tmpPN.chop(2);


            if (flags->flags() & Flags::Synchronous) {
                toInsert = "QString " + wsName + "::" + s + flags->methodSuffix()
                           + "(" + tmpP + ")" + flags->endLine()
                           + "{" + flags->endLine() + flags->tab()
                           + "// TODO: You can add your own data handling here, "
                           + "and make the whole method return" + flags->endLine()
                           + flags->tab() + "//       proper type." + flags->endLine();

                if (flags->flags() & Flags::NoMessagesStructure) {
                    toInsert += flags->tab() + "QMap<QString, QVariant> parameters;"
                            + flags->endLine();

                    foreach (QString param, tempParam.keys()) {
                        toInsert += flags->tab() + "parameters.insert(\"" + param
                                + "\", QVariant(" + param + "));" + flags->endLine();
                    }

                    toInsert += flags->endLine() + flags->tab()
                            + "return QWebServiceMethod::invokeMethod(this"
                            + ", QUrl(\"" + m->host() + "\"), \""
                            + m->methodName() + "\", parameters";

                    QString protocols = "QWebServiceMethod::"
                            + flags->protocolString(false);

                    if (protocols != "QWebServiceMethod::")
                        protocols.append(", ");

                    if (flags->flags() & Flags::Rest) {
                        protocols += "QWebServiceMethod::"
                                + flags->httpMethodString();
                    }

                    if (protocols != "")
                        protocols.prepend(", ");

                    toInsert += protocols + ").toString();" + flags->endLine();
                } else {
                    toInsert += flags->tab() + "return " + m->methodName()
                        + "::invokeMethod(this";

                    if (tmpPN != "")
                        toInsert += ", " + tmpPN + ");" + flags->endLine();
                    else
                        toInsert += ");" + flags->endLine();
                }
                toInsert += "}" + flags->endLine();
                serviceSource.insert(beginIndex, toInsert);
                beginIndex += toInsert.length();
            } else if (flags->flags() & Flags::Asynchronous) {
                // Name of the method object.
                QString objName = s.toLower() + flags->objectSuffix();
                toInsert = "void " + wsName + "::" + s + flags->methodSuffix()
                        + "(" + tmpP + ")" + flags->endLine()
                        + "{" + flags->endLine();

                if (flags->flags() & Flags::NoMessagesStructure) {
                    toInsert += flags->tab() + "QMap<QString, QVariant> parameters;"
                            + flags->endLine();

                    foreach (QString param, tempParam.keys()) {
                        toInsert += flags->tab() + "parameters.insert(\"" + param
                                + "\", QVariant(" + param + "));" + flags->endLine();
                    }

                    toInsert += flags->endLine()
                            + flags->tab() + objName + ".setHost(\"" + m->host()
                            + "\");" + flags->endLine()
                            + flags->tab() + objName + ".setTargetNamespace(\""
                            + m->targetNamespace() + "\");" + flags->endLine()
                            + flags->tab() + objName + ".setmethodName(\""
                            + m->methodName() + "\");" + flags->endLine();

                    QString protocols = "QWebServiceMethod::"
                            + flags->protocolString(false);

                    toInsert += flags->tab() + objName + ".setProtocol("
                            + protocols + ");" + flags->endLine();
                    // Add REST handling!

                    toInsert += flags->tab() + objName + ".invokeMethod(parameters);"
                            + flags->endLine();
                } else {
                    toInsert += flags->tab() + objName + ".invokeMethod(" + tmpPN
                            + ");" + flags->endLine();
                }
                toInsert += "}" + flags->endLine();
                serviceSource.insert(beginIndex, toInsert);
                beginIndex += toInsert.length();
            }
        }
    }

    beginIndex = logic.removeTag(serviceSource, "%methodReplyRead%");
    if (flags->flags() & Flags::Asynchronous) {
        // Define getters of methods' replies.
        toInsert = flags->tab() + "// Method reply getters: " + flags->endLine();
        foreach (QString s, tempMap->keys()) {
            /* Code not ready for compact mode checking.
            if (flags->flags() & Flags::compactMode)
            {
                ; // Code compact mode here :)
            }
            else if (!(flags->flags() & Flags::compactMode))
            */
            {
                QString tmpReturn;
                QWebMethod *m = tempMap->value(s);

                foreach (QString ret, m->returnValueNameType().keys()) {
                    tmpReturn = m->returnValueNameType().value(ret).typeName();
                    break; // This does not support multiple return values!
                }
                toInsert += tmpReturn + " " + wsName + "::" + s
                        + "ReplyRead()" + flags->endLine() + "{"
                        + flags->endLine() + flags->tab()
                        + "return " + s + "Result;" + flags->endLine()
                        + "}" + flags->endLine() + flags->endLine();
            }
        }
        serviceSource.insert(beginIndex, toInsert);
        beginIndex += toInsert.length();
    }

    beginIndex = logic.removeTag(serviceSource, "%methodReply%");
    if (flags->flags() & Flags::Asynchronous) {
        // Define all slots for asynchronous mode.
        foreach (QString s, tempMap->keys()) {
            /*
            if (flags->mode == Flags::compactMode)
            {
                ; // Code compact mode here :) Will probably be ust one method.
            }
            else
            */

            QString tmpReturn;
            QWebMethod *m = tempMap->value(s);

            foreach (QString ret, m->returnValueNameType().keys()) {
                tmpReturn = m->returnValueNameType().value(ret).typeName();
                break; // This does not support multiple return values!
            }
            toInsert = "void " + wsName + "::" + s + "Reply(QString result)" + flags->endLine()
                    + "{" + flags->endLine()
                    + flags->tab() + "// TODO: Add your own data handling here!" + flags->endLine()
                    + flags->tab() + "//" + s + "Result = your_new_value;" + flags->endLine()
                    + "}" + flags->endLine();
            serviceSource.insert(beginIndex, toInsert);
            beginIndex += toInsert.length();
        }
    }

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << serviceSource;
    // EOF (Web Service source)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal

    Orchestrator for method generation.
  */
bool CodeGenerator::createWebMethods()
{
    MethodGenerator mtdGen(methods, workingDir, flags, this);

    if (!mtdGen.createMethods())
        return enterErrorState(mtdGen.errorMessage());
    else
        return true;
}
/*!
    \internal

    Manages generation of various build system files (depending on flags set).
  */
bool CodeGenerator::createBuildSystemFile()
{
    bool result = false;

    // if (flags->flags() & Flags::noMessagesStructure)
    // qDebug() << "Remember to include QWebService library in your build system file!";

    if (flags->flags() & Flags::NoBuildSystem)
        return true;
    if (flags->flags() & Flags::Qmake)
        result = createQMakeProject();
    if (flags->flags() & Flags::Cmake)
        result = createCMakeProject();
    if (flags->flags() & Flags::Scons)
        result = createSconsProject();

    return result;
}

/*!
    \internal

    Generates QMake .pro file.
  */
bool CodeGenerator::createQMakeProject()
{
    QString wsName;
    if (baseClassName != QString())
        wsName = baseClassName;
    else
        wsName = wsdl->webServiceName();

    QFile file(QString(workingDir.path() + QLatin1String("/")
                       + wsName + QLatin1String(".pro")));
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState(QLatin1String("Error: could not open Web Service"
                                             ".pro file for writing."));

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "#-------------------------------------------------" << flags->endLine();
    out << "#" << flags->endLine();
    out << "# Project generated from WSDL by qtWsdlConverter ";
    out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") << flags->endLine();
//    out << "# Tomasz 'sierdzio' Siekierda" << flags->endLine();
//    out << "# sierdzio@gmail.com" << flags->endLine();
    out << "#-------------------------------------------------" << flags->endLine();
    out << flags->endLine();
    out << "QT += core network" << flags->endLine();
    out << "QT -= gui" << flags->endLine();
    out << flags->endLine();
    out << "TARGET = " << wsName << flags->endLine();
    out << flags->endLine();
    out << "TEMPLATE = app" << flags->endLine();
    out << flags->endLine();
    out << "SOURCES += ";
    // Create main.cpp to prevent compile errors. This file is NOT NEEDED in any other sense.
    if (!(flags->flags() & Flags::AllInOneDirStructure))
        out << "sources/";
    out << "main.cpp \\" << flags->endLine();
    if (!(flags->flags() & Flags::NoMessagesStructure)) {
        // Include all sources.
        QStringList tempMap = wsdl->methodNames();

        foreach (QString s, tempMap) {
            if (!(flags->flags() & Flags::AllInOneDirStructure))
                out << "sources/";
            out << s << ".cpp \\" << flags->endLine();
            out << flags->tab();
        }
    }
    if (!(flags->flags() & Flags::AllInOneDirStructure))
        out << "sources/";
    out << wsName << ".cpp" << flags->endLine();
    out << flags->endLine();
    out << flags->endLine();
    out << "HEADERS += ";
    if (!(flags->flags() & Flags::NoMessagesStructure))
    { // Include all headers.
        QStringList tempMap = wsdl->methodNames();

        foreach (QString s, tempMap) {            
            if (!(flags->flags() & Flags::AllInOneDirStructure))
                out << "headers/";
            out << s << ".h \\" << flags->endLine();
            out << flags->tab();
        }
    }
    if (!(flags->flags() & Flags::AllInOneDirStructure))
        out << "headers/";
    out << wsName << ".h";
    // EOF (QMake .pro file)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal

    Generates CMake's CMakeLists.txt file.
  */
bool CodeGenerator::createCMakeProject()
{
    QString wsName;
    if (baseClassName != QString())
        wsName = baseClassName;
    else
        wsName = wsdl->webServiceName();

    QFile file(QString(workingDir.path() + QLatin1String("/CMakeLists.txt")));
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState(QLatin1String("Error: could not open Web Service"
                                             ".pro file for writing."));

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "project(" << wsName << ")" << flags->endLine();
    out << "cmake_minimum_required(VERSION 2.4.0)" << flags->endLine();
    out << "find_package(Qt4 REQUIRED)" << flags->endLine();
    out << "include(${QT_USE_FILE})" << flags->endLine();
    // --------------------
    // Include sources:
    out << "set(" << wsName << "_SRCS" << flags->endLine();
    // Create main.cpp to prevent compile errors. This file is NOT NEEDED in any other sense.
    out << flags->tab();
    if (!(flags->flags() & Flags::AllInOneDirStructure))
        out << "sources/";
    out << "main.cpp" << flags->endLine();

    if (!(flags->flags() & Flags::NoMessagesStructure)) {
        // Include all sources.
        QStringList tempMap = wsdl->methodNames();

        foreach (QString s, tempMap) {
            if (!(flags->flags() & Flags::AllInOneDirStructure))
                out << "sources/";
            out << s << ".cpp" << flags->endLine();
            out << flags->tab();
        }
    }
    if (!(flags->flags() & Flags::AllInOneDirStructure))
        out << "sources/";
    out << wsName << ".cpp" << flags->endLine();
    out << ")" << flags->endLine();
    // --------------------
    // Include MOC:
    out << "set(" << wsName << "_MOC_SRCS" << flags->endLine();
    if (!(flags->flags() & Flags::NoMessagesStructure))
    { // Include all MOC headers.
        QStringList tempMap = wsdl->methodNames();

        foreach (QString s, tempMap) {
            if (!(flags->flags() & Flags::AllInOneDirStructure))
                out << "headers/";
            out << s << ".h" << flags->endLine();
            out << flags->tab();
        }
    }
    out << flags->tab();
    if (!(flags->flags() & Flags::AllInOneDirStructure))
        out << "headers/";
    out << wsName << ".h" << flags->endLine();
    out << ")" << flags->endLine();
    // Add compilation instructions:
    out << "qt4_wrap_cpp(" << wsName << "_MOCS ${" << wsName << "_MOC_SRCS})" << flags->endLine();
    out << "add_definitions(-DQT_NO_DEBUG)" << flags->endLine();
    out << "add_executable(" << wsName << flags->endLine();
    out << flags->tab() + "${" << wsName << "_SRCS}" << flags->endLine();
    out << flags->tab() + "${" << wsName << "_MOCS})" << flags->endLine();
    out << "target_link_libraries(" << wsName << " ${QT_LIBRARIES})" << flags->endLine();
    // EOF (CMake CMakeLists.txt file)
    // ---------------------------------

    file.close();

    return true;
}

/*!
    \internal

    Generates SCons' SConstruct file.
  */
bool CodeGenerator::createSconsProject()
{
    QString wsName;
    if (baseClassName != QString())
        wsName = baseClassName;
    else
        wsName = wsdl->webServiceName();

    QFile file(QString(workingDir.path() + QLatin1String("/SConstruct")));
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState(QLatin1String("Error: could not open Web Service"
                                             ".pro file for writing."));

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "#!/usr/bin/env python" << flags->endLine();
    out << flags->endLine();
    out << "import os" << flags->endLine();
    out << flags->endLine();
    out << "QT4_PY_PATH = \".\" # Probably not needed!" << flags->endLine();
    // WARNING! QTDIR has to be set on end machine!
    out << "QTDIR = \".\" # WARNING! Set QTDIR properly!" << flags->endLine();
    out << flags->endLine();
    out << "pkgpath = os.environ.get(\"PKG_CONFIG_PATH\", \"\")" << flags->endLine();
    out << "pkgpath += \":%s/lib/pkgconfig\" % QTDIR" << flags->endLine();
    out << "os.environ[\"PKG_CONFIG_PATH\"] = pkgpath" << flags->endLine();
    out << "os.environ[\"QTDIR\"] = QTDIR # WARNING! As above." << flags->endLine();
    out << flags->endLine();
    out << "env = Environment(tools=[\"default\", \"qt4\"], toolpath=[QT4_PY_PATH])"
        << flags->endLine();
    out << "env[\"CXXFILESUFFIX\"] = \".cpp\"" << flags->endLine();
    out << flags->endLine();
    out << "env.EnableQt4Modules([\"QtCore\", \"QtNetwork\"])" << flags->endLine();
    // --------------------
    // Include sources:
    out << "sources = [" << flags->endLine();
    // Create main.cpp to prevent compile errors. This file is NOT NEEDED in any other sense.
    out << flags->tab();
    if (!(flags->flags() & Flags::AllInOneDirStructure))
        out << "\"sources/";
    out << "main.cpp\"," << flags->endLine();

    if (!(flags->flags() & Flags::NoMessagesStructure)) {
        // Include all sources.
        QStringList tempMap = wsdl->methodNames();

        out << flags->tab();
        foreach (QString s, tempMap) {
            if (!(flags->flags() & Flags::AllInOneDirStructure))
                out << "\"sources/";
            out << s << ".cpp\"," << flags->endLine();
            out << flags->tab();
        }
        if (!(flags->flags() & Flags::AllInOneDirStructure))
            out << "\"sources/";
        out << wsName << ".cpp\"" << flags->endLine();
    } else {
        out << flags->tab();
        if (!(flags->flags() & Flags::AllInOneDirStructure))
            out << "\"sources/";
        out << wsName << ".cpp\"" << flags->endLine();
        out << "]" << flags->endLine();
    }
    out << "env.Program(target=\"" << wsName << "\", source=[sources])" << flags->endLine();
    // EOF (SCons SConstruct file)
    // ---------------------------------

    file.close();

//    qDebug() << "See the created SConstruct file for warnings on QTDIR and QT4 path.";
    return true;
}
