/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the qtWsdlConverter tool.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "../headers/wsdlconverter.h"

/*!
    \class WsdlConverter
    \brief Main class, supervizes code creation.

    Uses WSDL file or URL to transform WSDL code into set of Qt/C++ classes,
    according to options specified in Flags class.

    CodeGenerator class is used to create the actual code, while this class
    reads application switches, prepares the environment etc.
  */

/*!
    \enum WsdlConverter::ArgumentDescriptionFlag

    This enum type is used internally to mark app arguments:

    Mode:
    \value AppName
           Holds index of application name, if present.
    \value Path
           Holds index of application path.
    \value Dir
           Holds index of directory to create.
    \value ClassName
           Holds index of class name to use.
    \value FlagHelp
           Holds index of help flag.
*/

/*!
    Uses application's arguments (\a appArguments) to initialise
    QWsdl Flags and itself, and \a parent to construct the object.
  */
WsdlConverter::WsdlConverter(const QStringList &appArguments, QObject *parent) :
    QObject(parent)
{
    flags = new Flags();
    argList = new QMap<int, QVariant>();
    // Dummy wsdl to prevent segfaulting when parsing arguments fails.
    wsdl = new QWsdl(this);
    errorState = false;
//    errorMessage = "";

    QString applicationName = qApp->applicationFilePath().mid(
                qApp->applicationDirPath().length() + 1);

    if ((appArguments.length() == 0)
            || (appArguments.contains(QLatin1String("--help")))
            || (appArguments.contains(QLatin1String("-h")))
            || (appArguments.length() == 1
                && (appArguments.at(0) == qApp->applicationFilePath()
                    || appArguments.at(0) == (QLatin1String("./") + applicationName)
                    || appArguments.at(0) == applicationName)
                )) {
        displayHelp();
        return;
    }

    if (!parseArguments(appArguments)) {
        enterErrorState(QLatin1String("Encountered an error when parsing arguments."));
        return;
    }

    baseClassName = argList->value(ClassName).toString();
    outputDir = argList->value(Dir).toString();
    wsdl->setWsdlFile(argList->value(Path).toString());
    if (wsdl->isErrorState())
        enterErrorState(QLatin1String("WSDL error!"));
}

/*!
    Deletes the wsdl pointer.
  */
WsdlConverter::~WsdlConverter()
{
    delete wsdl;
    delete argList;
    delete flags;
}

/*!
    \fn WsdlConverter::errorEncountered(const QString &errMessage)

    Singal emitted when WsdlConverter encounters an error.
    Carries \a errMessage for convenience.
  */

/*!
    Sets the flags using \a flags object.
  */
void WsdlConverter::setFlags(Flags flags)
{
    flags = flags;
}

/*!
    Returns true if object is in error state.

    \sa errorInfo()
  */
bool WsdlConverter::isErrorState()
{
    return errorState;
}

/*!
    \internal

    Enters into error state with message \a errMessage.
  */
bool WsdlConverter::enterErrorState(const QString &errMessage)
{
    errorState = true;
    errorMessage += errMessage + QLatin1String("\n");
    emit errorEncountered(errMessage);
    return false;
}

/*!
    Returns error message or empty string, when no error was encountered.

    \sa isErrorState()
  */
QString WsdlConverter::errorInfo()
{
    return errorMessage;
}

/*!
  Resets the WsdlConverter object, removing the error state.
  */
void WsdlConverter::resetError()
{
    errorState = false;
    errorMessage.clear();
}

/*!
    Performs the WSDL => Qt/C++ code conversion.
  */
void WsdlConverter::convert()
{
    if (errorState) {
        enterErrorState(QLatin1String("Converter is in error state and cannot continue."));
        return;
    }

    displayIntro();
    QString mainPath = qApp->applicationDirPath()
            + QLatin1String("/") + webServiceName();
    QDir mainDir;
    if (outputDir != QString()) {
        if (outputDir.at(0) != QChar('/'))
            mainPath = qApp->applicationDirPath() + QLatin1String("/") + outputDir;
        else
            mainPath = outputDir;
    }
    mainDir.setPath(mainPath);

    if (mainDir.exists() && (flags->isForced() == false)) {
        // Might be good to add an interactive menu here
        enterErrorState(QLatin1String("Error - directory already exists! "
                                      "Use -f or --force to force deleting "
                                      "existing directories."));
        return;
    } else {
        if (flags->isForced() == true) {
            if(removeDir(mainPath)) {
                enterErrorState(QLatin1String("When using '--force': Removing "
                                "preexisting directory failed."));
                return;
            }
        }

        mainDir.mkdir(mainPath);
        mainDir.cd(mainPath);

        if (!CodeGenerator::create(wsdl, mainDir, flags, baseClassName, this)) {
            enterErrorState(QLatin1String("Error - code creation failed."));
            return;
        }
    }
    displayOutro();
    return;
}

/*!
    Returns web service's name.
  */
QString WsdlConverter::webServiceName()
{
    return wsdl->webServiceName();
}

/*!
    \internal

    Recursively removes directories.
  */
bool WsdlConverter::removeDir(const QString &path)
{
    QDir dir(path);
    bool err = false;
    if (dir.exists()) {
        QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot
                                                  | QDir::Dirs
                                                  | QDir::Files);

        int count = entries.size();
        for (int i = 0; ((i < count) && (err == false)); i++) {
            QFileInfo entryInfo = entries[i];
            QString tpath = entryInfo.absoluteFilePath();

            if (entryInfo.isDir()) {
                err = removeDir(tpath);
            } else {
                QFile file(tpath);
                if (!file.remove())
                    err = true;
            }
        }

        if (!dir.rmdir(dir.absolutePath()))
            err = true;
    }
    return(err);
}

/*!
    \internal

    Reads application's command line, sets Flags, paths etc.
  */
bool WsdlConverter::parseArguments(const QStringList &arguments)
{
    bool wasFile = false;
    bool wasOutDir = false;
    bool wasClassName = false;
    bool endOfOptions = false;
    QString appFilePath = qApp->applicationFilePath();

    foreach (QString s, arguments) {
        // Handles '--' arguments
        if (s.startsWith(QLatin1String("--")) && (endOfOptions == false)) {
            // End of options:
            if (s == QLatin1String("--")) {
                endOfOptions = true;
                continue;
            } else if (s == QLatin1String("--soap12")) { // Protocol flags:
                flags->resetFlags(Flags::Soap10 | Flags::Http
                                  | Flags::Json | Flags::Xml);
                flags->setFlags(Flags::Soap12);
            } else if (s == QLatin1String("--soap10")) {
                flags->resetFlags(Flags::Soap12 | Flags::Http
                                  | Flags::Json | Flags::Xml);
                flags->setFlags(Flags::Soap10);
            } else if (s == QLatin1String("--soap")) {
                flags->resetFlags(Flags::Http | Flags::Json | Flags::Xml);
                flags->setFlags(Flags::Soap);
            } else if (s == QLatin1String("--http")) {
                flags->resetFlags(Flags::Soap | Flags::Json | Flags::Xml);
                flags->setFlags(Flags::Http);
            } else if (s == QLatin1String("--json")) {
                flags->resetFlags(Flags::Soap | Flags::Http | Flags::Xml);
                flags->setFlags(Flags::Json);
            } else if (s == QLatin1String("--xml")) {
                flags->resetFlags(Flags::Soap | Flags::Http | Flags::Json);
                flags->setFlags(Flags::Xml);
            } else if (s.startsWith(QLatin1String("--rest"))) {
                flags->setFlags(Flags::Rest);
                // Set HTTP method:
                if (s == QLatin1String("--rest")) {
                    flags->setHttpMethod(Flags::Post);
                } else if (s.startsWith(QLatin1String("--rest="))) {
                    if (!flags->setHttpMethod(s.mid(7)))
                        return false;
                }
            } else if (s == QLatin1String("--synchronous")) { // Synchronousness:
                flags->resetFlags(Flags::Asynchronous);
                flags->setFlags(Flags::Synchronous);
            } else if (s == QLatin1String("--asynchronous")) {
                flags->resetFlags(Flags::Synchronous);
                flags->setFlags(Flags::Asynchronous);
            } else if (s == QLatin1String("--subclass")) { // Modes:
                flags->resetFlags(Flags::DebugMode
                                  | Flags::CompactMode
                                  | Flags::FullMode);
                flags->setFlags(Flags::Subclass);
            } else if ((s == QLatin1String("--full-mode"))
                        || (s == QLatin1String("--full"))) {
                flags->resetFlags(Flags::DebugMode
                                  | Flags::CompactMode
                                  | Flags::Subclass);
                flags->setFlags(Flags::FullMode);
            } else if ((s == QLatin1String("--debug-mode"))
                        || (s == QLatin1String("--debug"))) {
                flags->resetFlags(Flags::FullMode
                                  | Flags::CompactMode
                                  | Flags::Subclass);
                flags->setFlags(Flags::DebugMode);
            } else if ((s == QLatin1String("--compact-mode"))
                        || (s == QLatin1String("--compact"))) {
                flags->resetFlags(Flags::FullMode
                                  | Flags::CompactMode
                                  | Flags::Subclass);
                flags->setFlags(Flags::CompactMode);
            } else if ((s == QLatin1String("--standard-structure"))
                       || (s == QLatin1String("--standard"))) {
                // Structures:
                flags->resetFlags(Flags::NoMessagesStructure
                                  | Flags::AllInOneDirStructure);
                flags->setFlags(Flags::StandardStructure);
            } else if ((s == QLatin1String("--no-messages-structure"))
                       || (s == QLatin1String("--no-messages"))) {
                flags->resetFlags(Flags::StandardStructure
                                  | Flags::AllInOneDirStructure);
                flags->setFlags(Flags::NoMessagesStructure);
            } else if ((s == QLatin1String("--all-in-one-dir-structure"))
                     || (s == QLatin1String("--all-in-one-dir"))) {
                flags->resetFlags(Flags::StandardStructure
                                  | Flags::NoMessagesStructure);
                flags->setFlags(Flags::AllInOneDirStructure);
            } else if (s == QLatin1String("--qmake")) {
                // Build systems (qmake, cmake and scons can be build simultaneously):
                flags->setFlags(Flags::Qmake);
            } else if (s == QLatin1String("--cmake")) {
                flags->setFlags(Flags::Cmake);
            } else if (s == QLatin1String("--scons")) {
                flags->setFlags(Flags::Scons);
            } else if (s == QLatin1String("--no-build-system")) {
                flags->resetFlags(Flags::Qmake | Flags::Cmake | Flags::Scons);
                flags->setFlags(Flags::NoBuildSystem);
            } else if (s.startsWith(QLatin1String("--msgSuffix="))) {
                // Suffixes:
                flags->setMsgSuffix(s.mid(12));
            } else if (s.startsWith(QLatin1String("--objSuffix="))) {
                flags->setObjSuffix(s.mid(12));
            } else if (s == QLatin1String("--force")) { // Force:
                flags->setForced(true);
            } else {
                qWarning() << "WARNING: unrecognised command: "
                           << s << ". Converter will continue.";
            }
        } else if (s.startsWith(QLatin1String("-")) && (endOfOptions == false)) {
            // Handles '-' arguments
            for (int i = 1; i < s.size(); i++) {
                QChar chr = s.at(i);

                if (chr == ('a')) {
                    flags->resetFlags(Flags::Synchronous);
                    flags->setFlags(Flags::Asynchronous);
                } else if (chr == ('s')) {
                    flags->resetFlags(Flags::DebugMode
                                      | Flags::CompactMode
                                      | Flags::FullMode);
                    flags->setFlags(Flags::Subclass);
                } else if (chr == ('n')) {
                    flags->resetFlags(Flags::Qmake
                                      | Flags::Cmake
                                      | Flags::Scons);
                    flags->setFlags(Flags::NoBuildSystem);
                } else if (chr == ('f')) {
                    flags->setForced(true);
                } else {
                    qWarning() << "WARNING: unrecognised command: "
                               << s << ". Converter will continue.";
                }
            }
        } else if ((s != QString()) && (s != appFilePath)) {
            // Handles file names, class name etc.
            // Handles wsdl file, base class name, output dir.
            if (!wasFile) {
                wasFile = true;
                QString tmp = s;
                QUrl tempUrl(tmp);

                if (!QFile::exists(tmp) && tempUrl.isValid()) {
                    argList->insert(Path, tmp);;
                } else {
                    QFileInfo tempInfo(tmp);

                    if (tempInfo.isRelative()) {
                        tmp.prepend(qApp->applicationDirPath() + "/");
                        argList->insert(Path, tmp);
                    }
                }
            } else if (!wasOutDir) {
                wasOutDir = true;
                argList->insert(Dir, s);
            } else if (!wasClassName) {
                wasClassName = true;
                argList->insert(ClassName, s);
            }
        }
    }

    if (wasFile == false) {
        enterErrorState(QLatin1String("No WSDL file specified, conversion can no continue. "
                        "For help, type wsdlConvert --help."));
        return false;
    }

    return true;
}

/*!
    \internal

    Current implementation is not very nice.
    It simply throws out all the info into successive lines.
    No formatting is used.
*/
void WsdlConverter::displayHelp()
{
    QString helpMessage = QLatin1String("wsdlConvert - help.\n\n"
    "qtwsdlconvert [options] <WSDL file or URL> [output directory] "
    "[base output class name, defaults to web service name]\n"
    "Possible options: "
    "--soap10, --soap12, --http, --synchronous, --asynchronous, --help.\n"
    "New ones: --full-mode, --debug-mode, --compact-mode,\n"
    "--standard-structure, --no-messages-structure, --all-in-one-dir-structure,\n"
    "--qmake, --cmake, --scons, --no-build-system, --force, --json\n"
    "Default switches are: "
    "--synchronous, --soap12, --standard-structure, --full-mode, --qmake.\n\n"
    "qtWsdlConverter Copyright (C) 2011  Tomasz 'sierdzio' Siekierda\n"
    "This program comes with ABSOLUTELY NO WARRANTY.\n"
    "This is free software, and you are welcome to redistribute it\n"
    "under certain conditions, listed in LICENCE.txt.");
    enterErrorState(helpMessage);
}

/*!
    \internal

    Current implementation is not very nice.
    Shows intro, before convertion commences.
*/
void WsdlConverter::displayIntro()
{
//    qDebug() << "Creating code for web service:" << webServiceName();
//    if (argList->value(Dir).toString() == "")
//        qDebug() << "Output dir not specified. Defaulting to web service name ("
//                 << webServiceName() << ").";
}

/*!
  \internal

  Current implementation is not very nice.
  Displays outro, after convertion finishes.
*/
void WsdlConverter::displayOutro()
{
    if (isErrorState())
        qDebug() << "Conversion encountered an error.";
//    else
//        qDebug() << "Conversion successful.";
}
