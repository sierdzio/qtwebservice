#include "../headers/wsdlconverter.h"

WsdlConverter::WsdlConverter(QString wsdlFileOrUrl, QObject *parent, QString outputD, QString baseOutputCls) :
    QObject(parent), outputDir(outputD), baseClassName(baseOutputCls)
{
    errorState = false;
    errorMessage = "";
    wsdl = new QWsdl(wsdlFileOrUrl, this);

    if (wsdl->isErrorState())
        enterErrorState("WSDL error!");
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

bool WsdlConverter::enterErrorState(QString errMessage)
{
    errorState = true;
    errorMessage += errMessage + " ";
    qDebug() << errMessage;
    emit errorEncountered(errMessage);
    return false;
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
    if (outputDir != "")
    {
        if (outputDir.at(0) != '/')
            mainPath = qApp->applicationDirPath() + "/" + outputDir;
        else
            mainPath = outputDir;
    }
    mainDir.setPath(mainPath);

    if (mainDir.exists() && (flags.force == false))
    {
        // Might be good to add an interactive menu here (to ask for a new dir name)
        enterErrorState("Error - directory already exists!");

        return;
    }
    else
    {
        if (flags.force == true)
        {
            if(removeDir(mainPath))
            {
                enterErrorState("When using '--force': Removing preexisting directory failed.");
                return;
            }
        }

        mainDir.mkdir(mainPath);
        mainDir.cd(mainPath);

        if (flags.structure == Flags::standardStructure)
        {
            flags = Flags(Flags::fullMode, flags.synchronousness, flags.structure, flags.protocol, flags.buildSystem,
                          flags.force);
            if (!StandardPath::create(wsdl, mainDir, flags, baseClassName, this))
            {
                // Might be good to add an interactive menu here (to ask for a new dir name)
                enterErrorState("Error - code creation failed.");
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

bool WsdlConverter::removeDir(QString path)
{
    QDir dir(path);
    bool err = false;
    if (dir.exists())
    {
        QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot |
                                                   QDir::Dirs | QDir::Files);
        int count = entries.size();
        for (int i = 0; ((i < count) && (err == false)); i++)
        {
            QFileInfo entryInfo = entries[i];
            QString tpath = entryInfo.absoluteFilePath();
            if (entryInfo.isDir())
            {
                err = removeDir(tpath);
            }
            else
            {
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
