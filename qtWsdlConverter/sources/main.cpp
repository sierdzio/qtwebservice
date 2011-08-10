#include <QtCore>
#include <QtCore/QCoreApplication>
#include "../headers/flags.h"
#include "../headers/wsdlconverter.h"

enum argumentDescription {AppName, Path, Dir, ClassName, FlagHelp};
bool populateArgumentsList(QMap<int, QVariant> *lst, Flags *flgs);
void displayHelp();
void displayIntro(QMap<int, QVariant> *args, Flags flgs, WsdlConverter &converter);
void displayOutro(WsdlConverter &converter);

/**
  Reads flags, displays info, fires the converter.
  */
int main(int argc, char *argv[])
{    
    QCoreApplication a(argc, argv);
    Flags flags;
    QMap<int, QVariant> *args = new QMap<int, QVariant>();

    //TODO: check argument sanity, and whether .at(0) is app name or not. Partially done.
    if (!populateArgumentsList(args, &flags))
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
    converter.convert();
    displayIntro(args, flags, converter);
    displayOutro(converter);

    delete args;
    return 0; //a.exec();
}

/**
  Reads application's command line, sets Flags, paths etc.
  */
bool populateArgumentsList(QMap<int, QVariant> *lst, Flags *flgs)
{
    QStringList arguments = qApp->arguments();
    bool wasFile = false, wasOutDir = false, wasClassName = false;

    if (arguments.length() <= 1)
    {
        displayHelp();
        return false;
    }

    foreach (QString s, arguments)
    {
        // Handles '--' arguments
        if (s.startsWith("--"))
        {
            if (s == "--help")
            {
                displayHelp();
                return false;
            }
            // Protocol flags:
            else if (s == "--soap12")
            {
                flgs->resetFlags(Flags::soap10 | Flags::http | Flags::json);
                flgs->setFlags(Flags::soap12);
            }
            else if (s == "--soap10")
            {
                flgs->resetFlags(Flags::soap12 | Flags::http | Flags::json);
                flgs->setFlags(Flags::soap10);
            }
            else if (s == "--soap")
            {
                flgs->resetFlags(Flags::http | Flags::json);
                flgs->setFlags(Flags::soap);
            }
            else if (s == "--http")
            {
                flgs->resetFlags(Flags::soap | Flags::json);
                flgs->setFlags(Flags::http);
            }
            else if (s == "--json")
            {
                flgs->resetFlags(Flags::soap | Flags::http);
                flgs->setFlags(Flags::json);
            }
            // Synchronousness:
            else if (s == "--synchronous")
            {
                flgs->resetFlags(Flags::asynchronous);
                flgs->setFlags(Flags::synchronous);
            }
            else if (s == "--asynchronous")
            {
                flgs->resetFlags(Flags::synchronous);
                flgs->setFlags(Flags::asynchronous);
            }
            // Modes:
            else if (s == "--full-mode")
            {
                flgs->resetFlags(Flags::debugMode | Flags::compactMode);
                flgs->setFlags(Flags::fullMode);
            }
            else if (s == "--debug-mode")
            {
                flgs->resetFlags(Flags::fullMode) | Flags::compactMode;
                flgs->setFlags(Flags::debugMode);
            }
            else if (s == "--compact-mode")
            {
                flgs->resetFlags(Flags::fullMode | Flags::compactMode);
                flgs->setFlags(Flags::compactMode);
            }
            // Structures:
            else if (s == "--standard-structure")
            {
                flgs->resetFlags(Flags::noMessagesStructure | Flags::allInOneDirStructure);
                flgs->setFlags(Flags::standardStructure);
            }
            else if (s == "--no-messages-structure")
            {
                flgs->resetFlags(Flags::standardStructure | Flags::allInOneDirStructure);
                flgs->setFlags(Flags::noMessagesStructure);
            }
            else if (s == "--all-in-one-dir-structure")
            {
                flgs->resetFlags(Flags::standardStructure | Flags::noMessagesStructure);
                flgs->setFlags(Flags::allInOneDirStructure);
            }
            // Build systems (qmake, cmake and scons can be build simultaneously):
            else if (s == "--qmake")
                flgs->setFlags(Flags::qmake);
            else if (s == "--cmake")
                flgs->setFlags(Flags::cmake);
            else if (s == "--scons")
                flgs->setFlags(Flags::scons);
            else if (s == "--no-build-system")
            {
                flgs->resetFlags(Flags::qmake | Flags::cmake | Flags::scons);
                flgs->setFlags(Flags::noBuildSystem);
            }
            // Force:
            else if (s == "--force")
                flgs->setForced(true);
        }
        else if ((s != "") && (s != qApp->applicationFilePath()))
        { // Handles wsdl file, base class name, output dir.
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
    qDebug() << "wsdlConvert - help.";
    qDebug() << "";
    qDebug() << "qtwsdlconvert [options] <WSDL file or URL> [output directory] [base output class name, defaults to web service name]";
    qDebug() << "Possible options: --soap10, --soap12, --http, --synchronous, --asynchronous, --help.";
    qDebug() << "New ones: --full-mode, --debug-mode, --compact-mode,";
    qDebug() << "--standard-structure, --no-messages-structure, --all-in-one-dir-structure,";
    qDebug() << "--qmake, --cmake, --scons, --no-build-system, --force, --json";
    qDebug() << "Default switches are: --synchronous, --soap12, --standard-structure, --full-mode, --qmake.";
    qDebug() << "";
    qDebug() << "qtWsdlConverter Copyright (C) 2011  Tomasz 'sierdzio' Siekierda";
    qDebug() << "This program comes with ABSOLUTELY NO WARRANTY.";
    qDebug() << "This is free software, and you are welcome to redistribute it";
    qDebug() << "under certain conditions, listed in LICENCE.txt.";
}

void displayIntro(QMap<int, QVariant> *args, Flags flgs, WsdlConverter &converter)
{
    qDebug() << "Creating code for web service:" << converter.getWebServiceName();
    if (args->value(Dir).toString() == "")
        qDebug() << "Output dir not specified. Defaulting to web service name.";
/*
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
*/
}

void displayOutro(WsdlConverter &converter)
{
    if (converter.isErrorState())
        qDebug() << "Conversion encountered an error.";
    else
        qDebug() << "Conversion successful.";
}
