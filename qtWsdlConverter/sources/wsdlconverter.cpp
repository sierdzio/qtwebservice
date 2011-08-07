#include "../headers/wsdlconverter.h"

/*!
    \class WsdlConverter
    \brief Main class, supervizes code creation.

    Uses WSDL file or URL to transform WSDL code into set of Qt/C++ classes, according to options specified in Flags class.
  */

/*!
    \fn WsdlConverter::WsdlConverter(QString wsdlFileOrUrl, QObject *parent, QString outputD, QString baseOutputCls)

    Uses \a wsdlFileOrUrl, \a outputD, \a baseOutputCls and \a parent to construct the object. Only the WSDL QString
    is compulsory.
  */
WsdlConverter::WsdlConverter(QString wsdlFileOrUrl, QObject *parent, QString outputD, QString baseOutputCls) :
    QObject(parent), outputDir(outputD), baseClassName(baseOutputCls)
{
    errorState = false;
    errorMessage = "";
    wsdl = new QWsdl(wsdlFileOrUrl, this);

    if (wsdl->isErrorState())
        enterErrorState("WSDL error!");
}

/*!
    \fn WsdlConverter::~WsdlConverter()

    Deletes the wsdl pointer.
  */
WsdlConverter::~WsdlConverter()
{
    delete wsdl;
}

/*!
    \fn WsdlConverter::errorEncountered(QString errMessage)

    Singal emitted when WsdlConverter encounters an error. Carries \a errMessage for convenience.
  */

/*!
    \fn WsdlConverter::setFlags(Flags flgs)

    Sets the flags using \a flgs object.
  */
void WsdlConverter::setFlags(Flags flgs)
{
    flags = flgs;
}

/*!
    \fn WsdlConverter::isErrorState()

    Returns true if object is in error state.
  */
bool WsdlConverter::isErrorState()
{
    return errorState;
}

/*!
    \internal
    \fn WsdlConverter::enterErrorState(QString errMessage)

    Enters into error state with message \a errMessage.
  */
bool WsdlConverter::enterErrorState(QString errMessage)
{
    errorState = true;
    errorMessage += errMessage + " ";
    qDebug() << errMessage;
    emit errorEncountered(errMessage);
    return false;
}

/*!
    \fn WsdlConverter::convert()

    Performs the WSDL => c++ code conversion.
  */
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

    if (mainDir.exists() && (flags.isForced() == false))
    {
        // Might be good to add an interactive menu here (to ask for a new dir name)
        enterErrorState("Error - directory already exists!");

        return;
    }
    else
    {
        if (flags.isForced() == true)
        {
            if(removeDir(mainPath))
            {
                enterErrorState("When using '--force': Removing preexisting directory failed.");
                return;
            }
        }

        mainDir.mkdir(mainPath);
        mainDir.cd(mainPath);

        if (flags.flags() &= Flags::standardStructure)
        {
            flags.setFlags(Flags::fullMode);

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

/*!
    \fn WsdlConverter::getWebServiceName()

    Returns web service's name.
  */
QString WsdlConverter::getWebServiceName()
{
    return wsdl->getWebServiceName();
}

/*!
    \internal
    \fn WsdlConverter::removeDir(QString path)

    Recursively removes directories.
  */
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
