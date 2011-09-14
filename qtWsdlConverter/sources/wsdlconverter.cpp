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
WsdlConverter::WsdlConverter(QStringList appArguments, QObject *parent) :
    QObject(parent)
{
    flags = new Flags();
    argList = new QMap<int, QVariant>();
    // Dummy wsdl to prevent segfaulting when parsing arguments fails.
    wsdl = new QWsdl(this);
    errorState = false;
    errorMessage = "";

    QString applicationName = qApp->applicationFilePath().mid(
                qApp->applicationDirPath().length() + 1);

    if ((appArguments.length() == 0)
            || (appArguments.contains("--help"))
            || (appArguments.contains("-h"))
            || (appArguments.length() == 1
                && (appArguments.at(0) == qApp->applicationFilePath()
                    || appArguments.at(0) == ("./" + applicationName)
                    || appArguments.at(0) == applicationName)
                )) {
        displayHelp();
        return;
    }

    if (!parseArguments(appArguments)) {
        enterErrorState("Encountered an error when parsing arguments.");
        return;
    }

    baseClassName = argList->value(ClassName).toString();
    outputDir = argList->value(Dir).toString();
    wsdl->setWsdlFile(argList->value(Path).toString());
    if (wsdl->isErrorState())
        enterErrorState("WSDL error!");
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
    \fn WsdlConverter::errorEncountered(QString errMessage)

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
bool WsdlConverter::enterErrorState(QString errMessage)
{
    errorState = true;
    errorMessage += errMessage + "\n";
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
    errorMessage = "";
}

/*!
    Performs the WSDL => Qt/C++ code conversion.
  */
void WsdlConverter::convert()
{
    if (errorState) {
        enterErrorState("Converter is in error state and cannot continue.");
        return;
    }

    displayIntro();
    QString mainPath = qApp->applicationDirPath() + "/" + webServiceName();
    QDir mainDir;
    if (outputDir != "") {
        if (outputDir.at(0) != '/')
            mainPath = qApp->applicationDirPath() + "/" + outputDir;
        else
            mainPath = outputDir;
    }
    mainDir.setPath(mainPath);

    if (mainDir.exists() && (flags->isForced() == false)) {
        // Might be good to add an interactive menu here
        enterErrorState("Error - directory already exists! Use -f or --force "
                        "to force deleting existing directories.");
        return;
    }
    else {
        if (flags->isForced() == true) {
            if(removeDir(mainPath)) {
                enterErrorState("When using '--force': Removing "
                                "preexisting directory failed.");
                return;
            }
        }

        mainDir.mkdir(mainPath);
        mainDir.cd(mainPath);

        if (!CodeGenerator::create(wsdl, mainDir, flags, baseClassName, this)) {
            enterErrorState("Error - code creation failed.");
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
bool WsdlConverter::removeDir(QString path)
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
            }
            else {
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
bool WsdlConverter::parseArguments(QStringList arguments)
{
    bool wasFile = false;
    bool wasOutDir = false;
    bool wasClassName = false;
    bool endOfOptions = false;
    QString appFilePath = qApp->applicationFilePath();

    foreach (QString s, arguments) {
        // Handles '--' arguments
        if (s.startsWith("--") && (endOfOptions == false)) {
            // End of options:
            if (s == "--") {
                endOfOptions = true;
                continue;
            }
            // Protocol flags:
            else if (s == "--soap12") {
                flags->resetFlags(Flags::soap10 | Flags::http
                                  | Flags::json | Flags::xml);
                flags->setFlags(Flags::soap12);
            }
            else if (s == "--soap10") {
                flags->resetFlags(Flags::soap12 | Flags::http
                                  | Flags::json | Flags::xml);
                flags->setFlags(Flags::soap10);
            }
            else if (s == "--soap") {
                flags->resetFlags(Flags::http | Flags::json | Flags::xml);
                flags->setFlags(Flags::soap);
            }
            else if (s == "--http") {
                flags->resetFlags(Flags::soap | Flags::json | Flags::xml);
                flags->setFlags(Flags::http);
            }
            else if (s == "--json") {
                flags->resetFlags(Flags::soap | Flags::http | Flags::xml);
                flags->setFlags(Flags::json);
            }
            else if (s == "--xml") {
                flags->resetFlags(Flags::soap | Flags::http | Flags::json);
                flags->setFlags(Flags::xml);
            }
            else if (s.startsWith("--rest")) {
                flags->setFlags(Flags::rest);
                // Set HTTP method:
                if (s == "--rest") {
                    flags->setHttpMethod(Flags::POST);
                }
                else if (s.startsWith("--rest=")) {
                    if (!flags->setHttpMethod(s.mid(7)))
                        return false;
                }
            }
            // Synchronousness:
            else if (s == "--synchronous") {
                flags->resetFlags(Flags::asynchronous);
                flags->setFlags(Flags::synchronous);
            }
            else if (s == "--asynchronous") {
                flags->resetFlags(Flags::synchronous);
                flags->setFlags(Flags::asynchronous);
            }
            // Modes:
            else if (s == "--subclass") {
                flags->resetFlags(Flags::debugMode
                                  | Flags::compactMode
                                  | Flags::fullMode);
                flags->setFlags(Flags::subclass);
            }
            else if ((s == "--full-mode") || (s == "--full")) {
                flags->resetFlags(Flags::debugMode
                                  | Flags::compactMode
                                  | Flags::subclass);
                flags->setFlags(Flags::fullMode);
            }
            else if ((s == "--debug-mode") || (s == "--debug")) {
                flags->resetFlags(Flags::fullMode
                                  | Flags::compactMode
                                  | Flags::subclass);
                flags->setFlags(Flags::debugMode);
            }
            else if ((s == "--compact-mode") || (s == "--compact")) {
                flags->resetFlags(Flags::fullMode
                                  | Flags::compactMode
                                  | Flags::subclass);
                flags->setFlags(Flags::compactMode);
            }
            // Structures:
            else if ((s == "--standard-structure") || (s == "--standard")) {
                flags->resetFlags(Flags::noMessagesStructure
                                  | Flags::allInOneDirStructure);
                flags->setFlags(Flags::standardStructure);
            }
            else if ((s == "--no-messages-structure") || (s == "--no-messages")) {
                flags->resetFlags(Flags::standardStructure
                                  | Flags::allInOneDirStructure);
                flags->setFlags(Flags::noMessagesStructure);
            }
            else if ((s == "--all-in-one-dir-structure") || (s == "--all-in-one-dir")) {
                flags->resetFlags(Flags::standardStructure
                                  | Flags::noMessagesStructure);
                flags->setFlags(Flags::allInOneDirStructure);
            }
            // Build systems (qmake, cmake and scons can be build simultaneously):
            else if (s == "--qmake") {
                flags->setFlags(Flags::qmake);
            }
            else if (s == "--cmake") {
                flags->setFlags(Flags::cmake);
            }
            else if (s == "--scons") {
                flags->setFlags(Flags::scons);
            }
            else if (s == "--no-build-system") {
                flags->resetFlags(Flags::qmake | Flags::cmake | Flags::scons);
                flags->setFlags(Flags::noBuildSystem);
            }
            // Suffixes:
            else if (s.startsWith("--msgSuffix=")) {
                flags->setMsgSuffix(s.mid(12));
            }
            else if (s.startsWith("--objSuffix=")) {
                flags->setObjSuffix(s.mid(12));
            }
            // Force:
            else if (s == "--force") {
                flags->setForced(true);
            }
            else {
                qWarning() << "WARNING: unrecognised command: "
                           << s << ". Converter will continue.";
            }
        }
        // Handles '-' arguments
        else if (s.startsWith("-") && (endOfOptions == false)) {
            for (int i = 1; i < s.size(); i++) {
                QChar chr = s.at(i);

                if (chr == ('a')) {
                    flags->resetFlags(Flags::synchronous);
                    flags->setFlags(Flags::asynchronous);
                }
                else if (chr == ('s')) {
                    flags->resetFlags(Flags::debugMode
                                      | Flags::compactMode
                                      | Flags::fullMode);
                    flags->setFlags(Flags::subclass);
                }
                else if (chr == ('n')) {
                    flags->resetFlags(Flags::qmake
                                      | Flags::cmake
                                      | Flags::scons);
                    flags->setFlags(Flags::noBuildSystem);
                }
                else if (chr == ('f')) {
                    flags->setForced(true);
                }
                else {
                    qWarning() << "WARNING: unrecognised command: "
                               << s << ". Converter will continue.";
                }
            }
        }
        // Handles file names, class name etc.
        else if ((s != "") && (s != appFilePath)) {
            // Handles wsdl file, base class name, output dir.
            if (!wasFile) {
                wasFile = true;
                QString tmp = s;
                QUrl tempUrl(tmp);

                if (!QFile::exists(tmp) && tempUrl.isValid()) {
                    argList->insert(Path, tmp);;
                }
                else {
                    QFileInfo tempInfo(tmp);

                    if (tempInfo.isRelative()) {
                        tmp.prepend(qApp->applicationDirPath() + "/");
                        argList->insert(Path, tmp);
                    }
                }
            }
            else if (!wasOutDir) {
                wasOutDir = true;
                argList->insert(Dir, s);
            }
            else if (!wasClassName) {
                wasClassName = true;
                argList->insert(ClassName, s);
            }
        }
    }

    if (wasFile == false) {
        enterErrorState("No WSDL file specified, conversion can no continue. "
                        "For help, type wsdlConvert --help.");
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
    QString helpMessage = "wsdlConvert - help.\n\n"
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
    "under certain conditions, listed in LICENCE.txt.";
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
