#include "../headers/wsdlconverter.h"

WsdlConverter::WsdlConverter(QString wsdlFileOrUrl, QObject *parent, QDir outputDir, QString baseOutputClsNme) :
    QObject(parent)
{
    errorState = false;
    errorMessage = "";
    wsdl = new QWsdl(wsdlFileOrUrl, this);

    // Setting default flags:
//    flags();

    if (wsdl->isErrorState())
        enterErrorState("WSDL error!");
    else
    {
        convert();
    }
}

WsdlConverter::~WsdlConverter()
{
    delete wsdl;
}

void WsdlConverter::setFlags(Flags flgs)
{
    flags = flgs;
}

bool WsdlConverter::isErrorState()
{
    return errorState;
}

void WsdlConverter::enterErrorState(QString errMessage)
{
    errorState = true;
    errorMessage = errMessage;
    emit errorEncountered(errMessage);
}

void WsdlConverter::convert()
{
    /*
      Algorithm - initial plan:
      1. Load messages.
      2. Check and create the directory.
      3. (optional, for new flags) Dive into specified code creation path.
      4. (for standard path) Create dirs 'headers' and 'sources'.
      5. Create headers for messages (yes, a big task in one point :) ).
      6. Create sources for messages.
      7. Create the QWebServiceReaderAbstract subclass's header and source, put it into
            suitable directory.
      8. Create <webServiceName>.pro file.
      */

    QString mainPath = qApp->applicationDirPath() + "/" + getWebServiceName();
    QDir mainDir;
    mainDir.setPath(mainPath);

    if (mainDir.exists())
    {
        QString tmp = "Error - directory already exists!";
        // Might be good to add an interactive menu here (to ask for a new dir name)
        qDebug() << tmp;
        enterErrorState(tmp);

        return;
    }
    else
    {
        mainDir.mkdir(mainPath);
        mainDir.cd(mainPath);

        if (flags.structure == Flags::standardStructure)
        {
            flags = Flags(Flags::fullMode, flags.synchronousness, flags.structure, flags.protocol);
            if (!StandardPath::create(wsdl, mainDir, flags, this))
            {
                QString tmp = "Error - code creation failed.";
                // Might be good to add an interactive menu here (to ask for a new dir name)
                qDebug() << tmp;
                enterErrorState(tmp);
                return;
            }
        }
    }
    return;
}

QString WsdlConverter::getWebServiceName()
{
    return wsdl->getWebServiceName();
}
