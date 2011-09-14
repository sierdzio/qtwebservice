#include "../headers/codegenerator.h"

/*!
    \class CodeGenerator
    \brief Creates code for communicating with web services.

    Generates code, based on WSDL information and user defined flags.

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
    errorMessage = "";
}

/*!
    \fn CodeGenerator::errorEncountered(QString errMessage)

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
bool CodeGenerator::enterErrorState(QString errMessage)
{
    errorState = true;
    errorMessage += errMessage + "\n";
    emit errorEncountered(errMessage);
    return false;
}

/*!
    \internal
  */
void CodeGenerator::prepare()
{
    if (!(flags->flags() & Flags::allInOneDirStructure)) {
        workingDir.mkdir("headers");
        workingDir.mkdir("sources");
    }

    messages = wsdl->methods();
}

/*!
    Performs the conversion in CodeGenerator. Data from WSDL (\a wsdl) is
    combined with options specified in flags (\a flags), and base class name
    (\a baseClassName) to create a complete set of classes in given directory
    (\a workingDir). For Qt reasons, \a parent is also needed, although
    it defaults to 0.

    Returns true if successful.
  */
bool CodeGenerator::create(QWsdl *wsdl, QDir workingDir, Flags *flags, QString baseClassName, QObject *parent)
{
    CodeGenerator obj(parent);
    obj.baseClassName = baseClassName;
    obj.flags = flags;
    obj.workingDir = workingDir;
    obj.wsdl = wsdl;
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
  */
bool CodeGenerator::createService()
{
    if (!(flags->flags() & Flags::allInOneDirStructure))
        workingDir.cd("headers");
    if (!createServiceHeader())
        return enterErrorState("Creating header for Web Service \""
                               + wsdl->webServiceName() + "\" failed!");

    if (!(flags->flags() & Flags::allInOneDirStructure)) {
        workingDir.cdUp();
        workingDir.cd("sources");
    }

    if (!createServiceSource())
        return enterErrorState("Creating source for Web Service \""
                               + wsdl->webServiceName() + "\" failed!");

    if (!(flags->flags() & Flags::allInOneDirStructure))
        workingDir.cdUp();
    return true;
}

/*!
    \internal
  */
bool CodeGenerator::createServiceHeader()
{
    QString wsName = "";
    QMap<QString, QWebServiceMethod *> *tempMap = wsdl->methods();

    if (baseClassName != "")
        wsName = baseClassName;
    else
        wsName = wsdl->webServiceName();

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
    out << "#include <QtCore>" << endl;
    out << "#include <QUrl>" << endl;
    if (!(flags->flags() & Flags::noMessagesStructure)) {
        // Include all messages.
        QStringList tempMp = wsdl->methodNames();

        foreach (QString s, tempMp) {
            out << "#include \"" << s << ".h\"" << endl;
        }
    }
    else
    {
//        out << "#include <QWebService/QWebServiceMethod.h>" << endl;
        out << "#include <QWebService>" << endl;
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
        foreach (QString s, tempMap->keys()) {
            QString tmpReturn = "", tmpP = "";
            QWebServiceMethod *m = tempMap->value(s);

            foreach (QString ret, m->returnValueNameType().keys()) {
                tmpReturn = m->returnValueNameType().value(ret).typeName();
                break; // This does not support multiple return values!
            }

            QMap<QString, QVariant> tempParam = m->parameterNamesTypes();
            // Create msgParameters (comma separated list)
            foreach (QString param, tempParam.keys()) {
                tmpP += QString(tempParam.value(param).typeName()) + " "
                        + param + ", ";
            }
            tmpP.chop(2);
            // Temporarily, all messages will return QString!
            if (flags->flags() & Flags::synchronous)
                out << "    QString ";
            else
                out << "    void ";
            out << s << flags->messageSuffix() << "(" << tmpP << ");" << endl;
        }
    }
    out << endl;
    out << "    QUrl getHostUrl();" << endl;
    out << "    QString getHost();" << endl;
    out << "    bool isErrorState();" << endl;
    if (flags->flags() & Flags::asynchronous) {
        // Declare getters of methods' replies.
        out << "    // Method reply getters: " << endl;

        foreach (QString s, tempMap->keys()) {
            /* Code not ready for compact mode checking.
            if (flags->flags() & Flags::compactMode)
            {
                ; // Code compact mode here :)
            }
            else if (flags->flags() & Flags::fullMode || flags->flags() & Flags::debugMode)
            */
            {
                QString tmpReturn = "";
                QWebServiceMethod *m = tempMap->value(s);
                foreach (QString ret, m->returnValueNameType().keys()) {
                    tmpReturn = m->returnValueNameType().value(ret).typeName();
                    break; // This does not support multiple return values!
                }
                out << "    " << tmpReturn << " " << s << "ReplyRead();" << endl;
            }
        }
        out << endl;
    }
    out << endl;
    out << "protected slots:" << endl;
    if (flags->flags() & Flags::asynchronous) {
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
                out << "    void " << s << "Reply(QString result);" << endl;
            }
        }
        out << endl;
    }
    out << "protected:" << endl;
    out << "    bool errorState;" << endl;
    out << "    QUrl hostUrl;" << endl;

    if (flags->flags() & Flags::asynchronous) {
        // Declare reply variables for asynchronous mode.
        out << "    // Message replies:" << endl;

        foreach (QString s, tempMap->keys()) {
            QString tmpReturn = "";
            QWebServiceMethod *m = tempMap->value(s);

            foreach (QString ret, m->returnValueNameType().keys()) {
                tmpReturn = m->returnValueNameType().value(ret).typeName();
                break; // This does not support multiple return values!
            }
            out << "    " << tmpReturn << " " << s << "Result;" << endl;
        }

        out << "    // Messages:" << endl;
        foreach (QString s, tempMap->keys()) {
            if (!(flags->flags() & Flags::noMessagesStructure))
                out << "    " << s << " ";
            else // sierdzio I don't particurarly like this implementation, will rethink it later.
                out << "    QWebServiceMethod ";

            out << s.toLower() << flags->objectSuffix() << ";" << endl;
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
  */
bool CodeGenerator::createServiceSource()
{
    QString wsName = "";
    QMap<QString, QWebServiceMethod *> *tempMap = wsdl->methods();
    if (baseClassName != "")
        wsName = baseClassName;
    else
        wsName = wsdl->webServiceName();

    QFile file(workingDir.path() + "/" + wsName + ".cpp");
    if (!file.open(QFile::WriteOnly | QFile::Text)) // Means \r\n on Windows. Might be a bad idea.
        return enterErrorState("Error: could not open Web Service source file for writing.");

    // ---------------------------------
    // Begin writing:
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << "#include \"";
    if (!(flags->flags() & Flags::allInOneDirStructure))
        out << "../headers/";
    out << wsName << ".h\"" << endl;
    out << endl;
    out << "" << wsName << "::" << wsName << "(QObject *parent)" << endl;
    out << "    : QObject(parent)" << endl;
    out << "{" << endl;

    if (flags->flags() & Flags::asynchronous) {
        // Connect signals and slots for asynchronous mode.

        foreach (QString s, tempMap->keys()) {
            out << "    connect(&" << s.toLower() << flags->objectSuffix()
                << ", SIGNAL(replyReady(QString)), this, SLOT(";
            /* Code not ready for compact mode checking.
            if (flags->flags() & Flags::compactMode)
            {
                ; // Code compact mode here :)
            }
            else if (flags->flags() & Flags::fullMode || flags->flags() & Flags::debugMode)
            */
            {
                out << s << "Reply(QString)";
            }

            out << "));" << endl;
        }
    }
    out << "    hostUrl.setHost(\"" << wsdl->host() << "\");" << endl;
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

        foreach (QString s, tempMap->keys()) {
            QWebServiceMethod *m = tempMap->value(s);
            out << "    result.append(\"" << m->methodName() << "\");" << endl;
        }
        out << "    return result;" << endl;
    }
    out << "}" << endl;
    out << endl;
    { // Define all messages (as wrappers for message classes).
        foreach (QString s, tempMap->keys()) {
            QString tmpReturn = "", tmpP = "", tmpPN = "";
            QWebServiceMethod *m = tempMap->value(s);

            foreach (QString ret, m->returnValueNameType().keys()) {
                tmpReturn = m->returnValueNameType().value(ret).typeName();
                break; // This does not support multiple return values!
            }

            QMap<QString, QVariant> tempParam = m->parameterNamesTypes();

            // Create msgParameters (comma separated list)
            foreach (QString param, tempParam.keys()) {
                tmpP += QString(tempParam.value(param).typeName()) + " "
                        + param + ", ";
                tmpPN += param + ", ";
            }
            tmpP.chop(2);
            tmpPN.chop(2);


            if (flags->flags() & Flags::synchronous) {
                // Temporarily, all messages will return QString!
                // out << tmpReturn << " " << wsName << "::" << s << "(" << tmpP << ")" << endl;
                out << "QString " << wsName << "::" << s << flags->messageSuffix()
                    << "(" << tmpP << ")" << endl;
                out << "{" << endl;
                out << "    // TODO: You can add your own data handling here, "
                    << "and make the whole method return" << endl;
                out << "    //       proper type." << endl;

                if (flags->flags() & Flags::noMessagesStructure) {
                    out << "    QMap<QString, QVariant> parameters;" << endl;

                    foreach (QString param, tempParam.keys()) {
                        out << "    parameters.insert(\"" << param;
                        out << "\", QVariant(" << param << "));" << endl;
                    }

                    out << endl;
                    out << "    return QWebServiceMethod::sendMessage(this";
                    out << ", QUrl(\"" << m->host() << "\"), \""
                        << m->methodName() << "\", parameters";

                    QString protocols = "QWebServiceMethod::"
                            + flags->protocolString(false);

                    if (protocols != "QWebServiceMethod::")
                        protocols.append(", ");

                    if (flags->flags() & Flags::rest) {
                        protocols += "QWebServiceMethod::"
                                + flags->httpMethodString();
                    }

                    if (protocols != "")
                        protocols.prepend(", ");

                    out << protocols << ").toString();" << endl;
                }
                else {
                    out << "    return " << m->methodName()
                        << "::sendMessage(this";

                    if (tmpPN != "")
                        out << ", " << tmpPN << ");" << endl;
                    else
                        out << ");" << endl;
                }
                out << "}" << endl;
                out << endl;
            }
            else if (flags->flags() & Flags::asynchronous) {
                // Name of the message object.
                QString objName = s.toLower() + flags->objectSuffix();
                out << "void " << wsName << "::" << s << flags->messageSuffix()
                    << "(" << tmpP << ")" << endl;
                out << "{" << endl;

                if (flags->flags() & Flags::noMessagesStructure) {
                    out << "    QMap<QString, QVariant> parameters;" << endl;

                    foreach (QString param, tempParam.keys()) {
                        out << "    parameters.insert(\"" << param;
                        out << "\", QVariant(" << param << "));" << endl;
                    }

                    out << endl;
                    out << "    " << objName << ".setHost(\"" << m->host()
                        << "\");" << endl;
                    out << "    " << objName << ".setTargetNamespace(\""
                        << m->targetNamespace() << "\");" << endl;
                    out << "    " << objName << ".setMessageName(\""
                        << m->methodName() << "\");" << endl;

                    QString protocols = "QWebServiceMethod::"
                            + flags->protocolString(false);

                    out << "    " << objName << ".setProtocol("
                        << protocols << ");" << endl;
                    // Add REST handling!

                    out << "    " << objName << ".sendMessage(parameters);" << endl;
                }
                else {
                    out << "    " << objName << ".sendMessage(" << tmpPN
                        << ");" << endl;
                }

                out << "}" << endl;
                out << endl;
            }
        }
    }

    if (flags->flags() & Flags::asynchronous) {
        // Define getters of methods' replies.
        out << "    // Method reply getters: " << endl;

        foreach (QString s, tempMap->keys()) {
            /* Code not ready for compact mode checking.
            if (flags->flags() & Flags::compactMode)
            {
                ; // Code compact mode here :)
            }
            else if (!(flags->flags() & Flags::compactMode))
            */
            {
                QString tmpReturn = "";
                QWebServiceMethod *m = tempMap->value(s);

                foreach (QString ret, m->returnValueNameType().keys()) {
                    tmpReturn = m->returnValueNameType().value(ret).typeName();
                    break; // This does not support multiple return values!
                }
                out << tmpReturn << " " << wsName << "::" << s
                    << "ReplyRead()" << endl;
                out << "{" << endl;
                out << "    return " << s << "Result;" << endl;
                out << "}" << endl;
                out << endl;
            }
        }
        out << endl;
    }

    if (flags->flags() & Flags::asynchronous) {
        // Define all slots for asynchronous mode.

        foreach (QString s, tempMap->keys()) {
            /*
            if (flags->mode == Flags::compactMode)
            {
                ; // Code compact mode here :) Will probably be ust one method.
            }
            else
            */

            QString tmpReturn = "";
            QWebServiceMethod *m = tempMap->value(s);

            foreach (QString ret, m->returnValueNameType().keys()) {
                tmpReturn = m->returnValueNameType().value(ret).typeName();
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
    out << "    return hostUrl.host();" << endl;
    out << "}" << endl;
    out << endl;
    out << "bool " << wsName << "::isErrorState()" << endl;
    out << "{" << endl;
    out << "    return errorState;" << endl;
    out << "}" << endl;
    out << endl;
    // EOF (Web Service source)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal
  */
bool CodeGenerator::createMessages()
{
    MessageGenerator msgGen(messages, workingDir, flags, this);

    if (!msgGen.createMessages())
        return enterErrorState(msgGen.errorMessage());
    else
        return true;
}
/*!
    \internal
  */
bool CodeGenerator::createBuildSystemFile()
{
    bool result = false;

    // if (flags->flags() & Flags::noMessagesStructure)
    // qDebug() << "Remember to include QWebService library in your build system file!";

    if (flags->flags() & Flags::noBuildSystem)
        return true;
    if (flags->flags() & Flags::qmake)
        result = createQMakeProject();
    if (flags->flags() & Flags::cmake)
        result = createCMakeProject();
    if (flags->flags() & Flags::scons)
        result = createSconsProject();

    return result;
}

/*!
    \internal
  */
bool CodeGenerator::createQMakeProject()
{
    QString wsName = "";
    if (baseClassName != "")
        wsName = baseClassName;
    else
        wsName = wsdl->webServiceName();

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
    // Create main.cpp to prevent compile errors. This file is NOT NEEDED in any other sense.
    if (!(flags->flags() & Flags::allInOneDirStructure))
        out << "sources/";
    out << "main.cpp \\" << endl;
    if (!(flags->flags() & Flags::noMessagesStructure)) {
        // Include all sources.
        QStringList tempMap = wsdl->methodNames();

        foreach (QString s, tempMap) {
            if (!(flags->flags() & Flags::allInOneDirStructure))
                out << "sources/";
            out << s << ".cpp \\" << endl;
            out << "    ";
        }
    }
    if (!(flags->flags() & Flags::allInOneDirStructure))
        out << "sources/";
    out << wsName << ".cpp" << endl;
    out << endl;
    out << endl;
    out << "HEADERS += ";
    if (!(flags->flags() & Flags::noMessagesStructure))
    { // Include all headers.
        QStringList tempMap = wsdl->methodNames();

        foreach (QString s, tempMap) {            
            if (!(flags->flags() & Flags::allInOneDirStructure))
                out << "headers/";
            out << s << ".h \\" << endl;
            out << "    ";
        }
    }
    if (!(flags->flags() & Flags::allInOneDirStructure))
        out << "headers/";
    out << wsName << ".h";
    // EOF (QMake .pro file)
    // ---------------------------------

    file.close();
    return true;
}

/*!
    \internal
  */
bool CodeGenerator::createCMakeProject()
{
    QString wsName = "";
    if (baseClassName != "")
        wsName = baseClassName;
    else
        wsName = wsdl->webServiceName();

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
    // Create main.cpp to prevent compile errors. This file is NOT NEEDED in any other sense.
    out << "    ";
    if (!(flags->flags() & Flags::allInOneDirStructure))
        out << "sources/";
    out << "main.cpp" << endl;

    if (!(flags->flags() & Flags::noMessagesStructure)) {
        // Include all sources.
        QStringList tempMap = wsdl->methodNames();

        foreach (QString s, tempMap) {
            if (!(flags->flags() & Flags::allInOneDirStructure))
                out << "sources/";
            out << s << ".cpp" << endl;
            out << "    ";
        }
    }
    if (!(flags->flags() & Flags::allInOneDirStructure))
        out << "sources/";
    out << wsName << ".cpp" << endl;
    out << ")" << endl;
    // --------------------
    // Include MOC:
    out << "set(" << wsName << "_MOC_SRCS" << endl;
    if (!(flags->flags() & Flags::noMessagesStructure))
    { // Include all MOC headers.
        QStringList tempMap = wsdl->methodNames();

        foreach (QString s, tempMap) {
            if (!(flags->flags() & Flags::allInOneDirStructure))
                out << "headers/";
            out << s << ".h" << endl;
            out << "    ";
        }
    }
    out << "    ";
    if (!(flags->flags() & Flags::allInOneDirStructure))
        out << "headers/";
    out << wsName << ".h" << endl;
    out << ")" << endl;
    // Add compilation instructions:
    out << "qt4_wrap_cpp(" << wsName << "_MOCS ${" << wsName << "_MOC_SRCS})" << endl;
    out << "add_definitions(-DQT_NO_DEBUG)" << endl;
    out << "add_executable(" << wsName << endl;
    out << "    ${" << wsName << "_SRCS}" << endl;
    out << "    ${" << wsName << "_MOCS})" << endl;
    out << "target_link_libraries(" << wsName << " ${QT_LIBRARIES})" << endl;
    // EOF (CMake CMakeLists.txt file)
    // ---------------------------------

    file.close();

    return true;
}

/*!
    \internal
  */
bool CodeGenerator::createSconsProject()
{
    QString wsName = "";
    if (baseClassName != "")
        wsName = baseClassName;
    else
        wsName = wsdl->webServiceName();

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
    out << "env = Environment(tools=[\"default\", \"qt4\"], toolpath=[QT4_PY_PATH])"
        << endl;
    out << "env[\"CXXFILESUFFIX\"] = \".cpp\"" << endl;
    out << endl;
    out << "env.EnableQt4Modules([\"QtCore\", \"QtNetwork\"])" << endl;
    // --------------------
    // Include sources:
    out << "sources = [" << endl;
    // Create main.cpp to prevent compile errors. This file is NOT NEEDED in any other sense.
    out << "    ";
    if (!(flags->flags() & Flags::allInOneDirStructure))
        out << "\"sources/";
    out << "main.cpp\"," << endl;

    if (!(flags->flags() & Flags::noMessagesStructure)) {
        // Include all sources.
        QStringList tempMap = wsdl->methodNames();

        out << "    ";
        foreach (QString s, tempMap) {
            if (!(flags->flags() & Flags::allInOneDirStructure))
                out << "\"sources/";
            out << s << ".cpp\"," << endl;
            out << "    ";
        }
        if (!(flags->flags() & Flags::allInOneDirStructure))
            out << "\"sources/";
        out << wsName << ".cpp\"" << endl;
    }
    else {
        out << "    ";
        if (!(flags->flags() & Flags::allInOneDirStructure))
            out << "\"sources/";
        out << wsName << ".cpp\"" << endl;
        out << "]" << endl;
    }
    out << "env.Program(target=\"" << wsName << "\", source=[sources])" << endl;
    // EOF (SCons SConstruct file)
    // ---------------------------------

    file.close();

//    qDebug() << "See the created SConstruct file for warnings on QTDIR and QT4 path.";
    return true;
}
