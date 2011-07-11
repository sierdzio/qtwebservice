#include <QtCore>
#include <QtCore/QCoreApplication>
#include "../headers/flags.h"
#include "../headers/wsdlconverter.h"

enum argumentDescription {AppName, Path, Dir, ClassName, FlagHelp};
bool populateArgumentsList(QMap<int, QVariant> *lst, Flags flgs);
void displayHelp();
void displayIntro(QMap<int, QVariant> *args, Flags flgs, WsdlConverter &converter);
void displayOutro(WsdlConverter &converter);

int main(int argc, char *argv[])
{    
    QCoreApplication a(argc, argv);
    Flags flags;
    QMap<int, QVariant> *args = new QMap<int, QVariant>();

    //TODO: check argument sanity, and whether .at(0) is app name or not. Partially done.
    if (!populateArgumentsList(args, flags))
    {
        delete args;
        return 1;
    }

    WsdlConverter converter(args->value(Path).toString(),
                            0,
                            args->value(Dir).toString(),
                            args->value(ClassName).toString());

    // Set flags:
    converter.setFlags(flags);

    // Doing the conversion:
    displayIntro(args, flags, converter);
    displayOutro(converter);

    delete args;
    return 0; //a.exec();
}

bool populateArgumentsList(QMap<int, QVariant> *lst, Flags flgs)
{
    /*
        qtwsdlconvert [options] <WSDL file or URL> [output directory, defaults to web service name] [base output class name, defaults to web service name]

        Possible options: --soap10, --soap12, --http, --synchronous, --asynchronous, --help.
        New ones: --full-mode, --debug-mode, --compact-mode,
                  --standard-structure, --no-messages-structure, --all-in-one-dir-structure.
        Upcoming: --qmake, --cmake, --scons, --no-build-system.

        --synchronous, --soap12, --standard-structure, --full-mode, --qmake switches are default ones.
    */

    QStringList arguments = qApp->arguments();
    bool wasFile = false, wasOutDir = false, wasClassName = false;

    if (arguments.length() <= 1)
    {
        displayHelp();
        return false;
    }

    foreach (QString s, arguments)
    {
        if (s.startsWith("--"))
        {
            if (s == "--help")
            {
                displayHelp();
                return false;
            }
            // Protocol flags:
            else if (s == "--soap12")
                flgs.protocol = QSoapMessage::soap12;
            else if (s == "--soap10")
                flgs.protocol = QSoapMessage::soap10;
            else if (s == "--html")
                flgs.protocol = QSoapMessage::http;
            // Synchronousness:
            else if (s == "--synchronous")
                flgs.synchronousness = Flags::synchronous;
            else if (s == "--asynchronous")
                flgs.synchronousness = Flags::asynchronous;
            // Modes:
            else if (s == "--full-mode")
                flgs.mode = Flags::fullMode;
            else if (s == "--debug-mode")
                flgs.mode = Flags::debugMode;
            else if (s == "--compact-mode")
                flgs.mode = Flags::compactMode;
            // Structures:
            else if (s == "--standard-structure")
                flgs.structure = Flags::standardStructure;
            else if (s == "--no-messages-structure")
                flgs.structure = Flags::noMessagesStructure;
            else if (s == "--all-in-one-dir-structure")
                flgs.structure = Flags::allInOneDirStructure;
            // Build systems:
            else if (s == "--qmake")
                flgs.buildSystem = Flags::qmake;
            else if (s == "--cmake")
                flgs.buildSystem = Flags::cmake;
            else if (s == "--scons")
                flgs.buildSystem = Flags::scons;
            else if (s == "--no-build-system")
                flgs.buildSystem = Flags::noBuildSystem;
        }
        else if ((s != "") && (s != qApp->applicationFilePath()))
        {
            if (!wasFile)
            {
                wasFile = true;
                QString tmp = s;
                QUrl tempUrl(tmp);
                if (!QFile::exists(tmp) && tempUrl.isValid())
                {
                    lst->insert(Path, tmp);;
                }
                else
                {
                    QFileInfo tempInfo(tmp);
                    if (tempInfo.isRelative())
                    {
                        tmp.prepend(qApp->applicationDirPath() + "/");
                        lst->insert(Path, tmp);
                    }
                }
            }
            else if (!wasOutDir)
            {
                wasOutDir = true;
                lst->insert(Dir, s);
            }
            else if (!wasClassName)
            {
                wasClassName = true;
                lst->insert(ClassName, s);
            }
        }
    }

    if (!lst->contains(Path))
    {
        qDebug() << "No WSDL file specified, conversion can no continue. For help, type wsdlConvert --help.";
        return false;
    }

    return true;
}

/*
  Current implementation is not very nice.

  It simply throws out all the info into successive lines. No formatting is used.
*/
void displayHelp()
{
//    QString hlpMsg = "";
//    qDebug() << "";
    qDebug() << "wsdlConvert - help.";
    qDebug() << "";
    qDebug() << "qtwsdlconvert [options] <WSDL file or URL> [output directory] [base output class name, defaults to web service name]";
//    qDebug() << "";
    qDebug() << "Possible options: --soap10, --soap12, --http, --synchronous, --asynchronous, --help.";
    qDebug() << "New ones: --fullMode, --debugMode, --compactMode,";
    qDebug() << "--standard-structure, --no-messages-structure, --all-in-one-dir-structure.";
    qDebug() << "Upcoming: --qmake, --cmake, --scons, --no-build-system";
    qDebug() << "Default switches are: --synchronous, --soap12, --standard-structure, --full-mode, --qmake.";
    qDebug() << "";
    qDebug() << "Copyright by Tomasz Siekierda <sierdzio@gmail.com>";
    qDebug() << "Distributed under GPLv3";
    qDebug() << "";
}

void displayIntro(QMap<int, QVariant> *args, Flags flgs, WsdlConverter &converter)
{
    qDebug() << "Creating code for web service:" << converter.getWebServiceName();
    if (args->value(Dir).toString() == "")
        qDebug() << "Output dir not specified. Defaulting to web service name.";

    QString tempFlags = "Using flags: ";
    if (flgs.synchronousness == Flags::synchronous)
        tempFlags += "synchronous, ";
    else
        tempFlags += "asynchronous, ";
    if (flgs.protocol == QSoapMessage::http)
        tempFlags += "http, ";
    else if (flgs.protocol == QSoapMessage::soap10)
        tempFlags += "soap10, ";
    else
        tempFlags += "soap12";
    qDebug() << tempFlags;
}

void displayOutro(WsdlConverter &converter)
{
    if (!converter.isErrorState())
        qDebug() << "Strange. Everything seems to be working just fine... Operation completed successfully.";
}
