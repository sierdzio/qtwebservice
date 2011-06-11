#include <QtCore>
#include <QtCore/QCoreApplication>
#include "../headers/wsdlconverter.h"

enum argumentDescription {AppName, Path, Dir, ClassName, FlagSyn, FlagProt, FlagHelp};
bool populateArgumentsList(QMap<int, QVariant> *lst);
void displayHelp();

int main(int argc, char *argv[])
{    
    QCoreApplication a(argc, argv);


//    QStringList arguments = a.arguments();
    QMap<int, QVariant> *args = new QMap<int, QVariant>();

    //TODO: check argument sanity, and whether .at(0) is app name or not.
    if (!populateArgumentsList(args))
        return 1;

    //    "/home/sierdzio/Dropbox/Code/QWebService/QWebService_main/QWebService/examples/band_ws.asmx");
    WsdlConverter converter(args->value(Path).toString(),
                            new QObject(),
                            QDir(args->value(Dir).toString()),
                            args->value(ClassName).toString());

    // Now that is a bit shaky implementation, I (sierdzio) recommend to change it later:
    converter.setFlags((WsdlConverter::Synchronousness) args->value(FlagSyn).toInt(),
                       (QSoapMessage::Protocol) args->value(FlagProt).toInt());

    if (!converter.isErrorState())
        qDebug() << "Strange. Everything seems to be working just fine...";

    return a.exec();
}

bool populateArgumentsList(QMap<int, QVariant> *lst)
{
    /*
        qtwsdlconvert [options] <WSDL file or URL> [output directory] [base output class name, defaults to web service name]

        Possible options: --soap10, --soap12, --http, --synchronous, --asynchronous, --help.

        --synchronous, --soap12 switches are default ones.
    */

    QStringList arguments = qApp->arguments();
    bool wasFile = false, wasOutDir = false, wasClassName = false;

    // Set default flags:
    lst->insert(FlagProt, QSoapMessage::soap12);
    lst->insert(FlagSyn, WsdlConverter::synchronous);

    foreach (QString s, arguments)
    {
        if (s.startsWith("--"))
        {
            if (s == "--soap12")
                lst->insert(FlagProt, QSoapMessage::soap12);
            else if (s == "--soap10")
                lst->insert(FlagProt, QSoapMessage::soap10);
            else if (s == "--html")
                lst->insert(FlagProt, QSoapMessage::http);
            else if (s == "--synchronous")
                lst->insert(FlagSyn, WsdlConverter::synchronous);
            else if (s == "--asynchronous")
                lst->insert(FlagSyn, WsdlConverter::asynchronous);
        }
        else if (s != "")
        {
            if (!wasFile)
            {
                wasFile = true;
                lst->insert(Path, s);
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
  Current implementation is very, very wrong.

  It simply throws out all the info in successive lines, instead of
  invoking any pleasant program, like more or less.
*/
void displayHelp()
{
//    QString hlpMsg = "";
    qDebug() << "wsdlConvert  - help.";
    qDebug() << "";
    qDebug() << "qtwsdlconvert [options] <WSDL file or URL> [output directory] [base output class name, defaults to web service name]";
    qDebug() << "";
    qDebug() << "Possible options: --soap10, --soap12, --http, --synchronous, --asynchronous, --help.";
    qDebug() << "";
    qDebug() << "Default switches are: --synchronous, --soap12.";
    qDebug() << "Copyright by Tomasz Siekierda <sierdzio@gmail.com>";
    qDebug() << "Distributed under <some GPL licence  -to be decided later>";
}
