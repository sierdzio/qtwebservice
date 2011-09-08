#include "../headers/qwsdl.h"

/*!
  \class QWsdl
  \brief Class for interaction with local and remote WSDL files. Currently read-only.

    Reads web service data (message names, parameters, return values, ws name etc) from a WSDL file or URL.
  */

/*!
    \fn QWsdl::QWsdl(QObject *parent)

    Simple constructor, requires \a parent only, but needs other information to be specified later
    in order to run. You need to run setWsdlFile(), which automatically parses the file.


    Initialises the whole object with default values.

    \sa setWsdlFile(), resetWsdl()
  */
QWsdl::QWsdl(QObject *parent) :
    QObject(parent)
{
    wsdlFilePath = QString::fromLatin1("");
    init();
}

/*!
    \fn QWsdl::QWsdl(QString wsdlFile, QObject *parent)

    Constructs the object using optional \a parent. Uses the file path or URL
    specified in \a wsdlFile to parse the WSDL file.
  */
QWsdl::QWsdl(QString wsdlFile, QObject *parent) :
    QObject(parent), wsdlFilePath(wsdlFile)
{
    init();
    parse();
}

/*!
    \fn QWsdl::~QWsdl()

    Destructor - cleans some internal variables.
  */
QWsdl::~QWsdl()
{
    delete methodsMap;
    delete workMethodList;
    delete workMethodParameters;
}

/*!
    \fn QWsdl::errorEncountered(QString errMessage)

    Singal emitted when WsdlConverter encounters an error. Carries \a errMessage for convenience.
  */

/*!
    \fn QWsdl::setWsdlFile(QString wsdlFile)

    Wrapper for resetWsdl(). Used to set the WSDL file or URL using \a wsdlFile. Compulsory after simple constructor,
    but not needed if you have already specified the file in the constructor or resetWsdl().

    \sa resetWsdl()
  */
void QWsdl::setWsdlFile(QString wsdlFile) // == resetWsdl()
{
    resetWsdl(wsdlFile);
}

/*!
    \fn QWsdl::resetWsdl(QString newWsdlPath)

    Can be used to set or reset a WSDL file (or URL), using \a newWsdlPath. Cleans and reinitialises
    the object, parses the file.

    \sa setWsdlFile()
  */
void QWsdl::resetWsdl(QString newWsdlPath)
{
    wsdlFilePath = newWsdlPath;

    methodsMap->clear();
    workMethodList->clear();
    workMethodParameters->clear();
    replyReceived = false;
    errorState = false;
    errorMessage = QString::fromLatin1("");
    m_webServiceName = QString::fromLatin1("");
    m_hostUrl.setUrl(QString::fromLatin1(""));
    m_targetNamespace = QString::fromLatin1("");
    xmlReader.clear();

    parse();
}

/*!
    \fn QWsdl::methodNames() const

    Returns a QStringList of names of web service's methods.

    \sa methods()
  */
QStringList QWsdl::methodNames() const
{
    QList<QString> tempMethods = methodsMap->keys();
    QStringList result;
    result.append(tempMethods);

    return result;
}

/*!
    \fn QWsdl::methods()

    Returns a QMap<QString, QWebServiceMethod *> pointer. Keys are method names (just as in
    getMethodNames()), and values are QWebServiceMethods themselves (which means they can be used
    not only to get information, but also to send messages, set them up etc.).

    \sa methodNames()
  */
QMap<QString, QWebServiceMethod *> *QWsdl::methods()
{
    return methodsMap;
}

/*!
    \fn QWsdl::webServiceName() const

    Returns QString with the name of the web service specified in WSDL.
  */
QString QWsdl::webServiceName() const
{
    return m_webServiceName;
}

/*!
    \fn QWsdl::host() const

    Returns web service's URL. If there is no valid URL in WSDL file, path to this file is returned.

    \sa hostUrl()
  */
QString QWsdl::host() const
{
    if (!m_hostUrl.isEmpty())
        return m_hostUrl.path();
    else
        return wsdlFilePath;
}

/*!
    \fn QWsdl::hostUrl() const

    Returns web service's URL. If there is no valid URL in WSDL file, path to this file is returned.

    \sa host()
  */
QUrl QWsdl::hostUrl() const
{
    if (!m_hostUrl.isEmpty())
        return m_hostUrl;
    else
        return QUrl(wsdlFilePath);
}

/*!
    \fn QWsdl::wsdlFile() const

    Returns path to WSDL file.
  */
QString QWsdl::wsdlFile() const
{
    return wsdlFilePath;
}

/*!
    \fn QWsdl::targetNamespace() const

    Returns target namespace specified in WSDL.
  */
QString QWsdl::targetNamespace() const
{
    return m_targetNamespace;
}

//QFile QWsdl::getWsdl()
//{

//}

/*!
    \fn QWsdl::errorInfo() const

    Returns QString with error message in case an error occured. Otherwise, returns empty string.

    \sa isErrorState()
  */
QString QWsdl::errorInfo() const
{
    return errorMessage;
}

/*!
    \fn QWsdl::isErrorState() const

    Returns true if there was an error, false otherwise. Details about an error can be read with
    getErrorInfo().

    \sa errorInfo()
  */
bool QWsdl::isErrorState() const
{
    return errorState;
}

/*!
    \fn QWsdl::fileReplyFinished(QNetworkReply *rply)

    Asynchronous public return slot. Reads WSDL reply (\a rply) from server (used in case
    URL was specified in wsdl file path).

    Not sure why it's public (sierdzio). Maybe it will go private in the future.
  */
void QWsdl::fileReplyFinished(QNetworkReply *rply)
{
    QString replyString = convertReplyToUtf(QString::fromLatin1(rply->readAll()));
    QFile file(QString::fromLatin1("tempWsdl.asmx~"));

    if (file.exists())
        file.remove();

    if (!file.open(QFile::WriteOnly)) {
        enterErrorState(QString("Error: cannot write WSDL file from remote location. Reason: " + file.errorString()));
        return;
    }
    else {
        file.write(replyString.toUtf8());
    }

    file.close();
    replyReceived = true;
    rply->deleteLater();
//    emit replyReady(reply);
}

/*!
  \internal
  \fn QWsdl::init()

  Initialises the object.
  */
void QWsdl::init()
{
    replyReceived = false;
    errorState = false;
    errorMessage = QString::fromLatin1("");
    m_webServiceName = QString::fromLatin1("");
    m_hostUrl.setUrl(QString::fromLatin1(""));
    m_targetNamespace = QString::fromLatin1("");

    workMethodList = new QStringList();
    workMethodParameters = new QMap<int, QMap<QString, QVariant> >();
    methodsMap = new QMap<QString, QWebServiceMethod *>;
}

/*!
    \internal
    \fn QWsdl::enterErrorState(QString errMessage)

    Enters into error state with message \a errMessage.
  */
bool QWsdl::enterErrorState(QString errMessage)
{
    errorState = true;
    errorMessage += QString(errMessage + " ");
//    qDebug() << errMessage;
    emit errorEncountered(errMessage);
    return false;
}

/*!
    \internal
    \fn QWsdl::parse()

    Central method of this class. Parses the WSDL file, creates all QWebServiceMethods,
    reads all necessary data, like web service's name etc.
  */
bool QWsdl::parse()
{
    /*
      WARNING!
      Namespace abbreviations are currently hardcoded!
      This needs to be changed before release.
      Real namespaces can be extracted from 'wsdl:definitions' tag attributes.

      Algorithm extracts method names from "types" tags, which is most probably wrong,
      as it should be cross-checked with other tags ("message", etc.)

      Most probably, use of QtXmlPatterns would solve a lot of that.
    */

    if (errorState) {
        enterErrorState(QString::fromLatin1("WSDL reader is in error state and cannot parse the file."));
        return false;
    }

    prepareFile();

    QFile file(wsdlFilePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        enterErrorState(QString("Error: cannot read WSDL file: " + wsdlFilePath + ". Reason: " + file.errorString()));
        return false;
    }

    xmlReader.setDevice(&file);
    xmlReader.readNext();

    while (!xmlReader.atEnd()) {
        if (xmlReader.isStartElement()) {
            QString tempN = xmlReader.name().toString();

            if (tempN == QString::fromLatin1("definitions")) {
                m_targetNamespace = xmlReader.attributes().value(QString::fromLatin1("targetNamespace")).toString();
                readDefinitions();
            }
            else {
                enterErrorState(QString::fromLatin1("Error: file does not have WSDL definitions inside!"));
                return false;
            }
        }
        else {
            xmlReader.readNext();
        }
    }

    prepareMethods();

    if (!errorState)
        return true;
    else
        return false;
}

/*!
    \internal

    If the host path is not a local file, but URL, QWsdl will download it into a temporary file,
    then read, and delete at exit.
  */
void QWsdl::prepareFile()
{
    QUrl filePath;
    filePath.setUrl(wsdlFilePath);

    if (!QFile::exists(wsdlFilePath) && filePath.isValid()) {
        m_hostUrl = filePath;

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(fileReplyFinished(QNetworkReply*)));

        manager->get(QNetworkRequest(filePath));

        forever {
            if (replyReceived == true)
                return;
            else
                QCoreApplication::instance()->processEvents();
        }
    }
}

/*!
    \internal
  */
void QWsdl::readDefinitions()
{
    //EXPERIMENTAL
    // QMap used to supress multiple "tag not supported yet" messages.
    // May actually evolve into more universal tag recognition and handling structure.
    QMap<QString, bool> tagUsed;
    tagUsed.insert(QString::fromLatin1("types"), false);
    tagUsed.insert(QString::fromLatin1("message"), false);
    tagUsed.insert(QString::fromLatin1("portType"), false);
    tagUsed.insert(QString::fromLatin1("binding"), false);
    tagUsed.insert(QString::fromLatin1("service"), false);
    tagUsed.insert(QString::fromLatin1("documentation"), false);
    //END of EXPERIMENTAL

    xmlReader.readNext();
    QString tempName = xmlReader.name().toString();

    while(!xmlReader.atEnd()) {
        tempName = xmlReader.name().toString();

        if (xmlReader.isEndElement() && (tempName == QString::fromLatin1("definitions"))) {
            xmlReader.readNext();
            break;
        }
        else if (xmlReader.isEndElement()) {
            xmlReader.readNext();
            continue;
        }

        if (tempName == QString::fromLatin1("types")) {
            readTypes();
//            tagUsed.insert("types", true);
            xmlReader.readNext();
        }
        else if (tempName == QString::fromLatin1("message")) {
            if (!tagUsed.value(QString::fromLatin1("message")))
                readMessages();
            tagUsed.insert(QString::fromLatin1("message"), true);
            xmlReader.readNext();
        }
        else if (tempName == QString::fromLatin1("portType")) {
            if (!tagUsed.value(QString::fromLatin1("portType")))
                readPorts();
            tagUsed.insert(QString::fromLatin1("portType"), true);
            xmlReader.readNext();
        }
        else if (tempName == QString::fromLatin1("binding")) {
            if (!tagUsed.value(QString::fromLatin1("binding")))
                readBindings();
            tagUsed.insert(QString::fromLatin1("binding"), true);
            xmlReader.readNext();
        }
        else if (tempName == QString::fromLatin1("service")) {
//            if (!tagUsed.value("service"))
                readService();
            tagUsed.insert(QString::fromLatin1("service"), true);
            xmlReader.readNext();
        }
        else if (tempName == QString::fromLatin1("documentation")) {
            if (!tagUsed.value(QString::fromLatin1("documentation")))
                readDocumentation();
            tagUsed.insert(QString::fromLatin1("documentation"), true);
            xmlReader.readNext();
        }
        else {
            xmlReader.readNext();
        }
    }
}

/*!
    \internal
  */
void QWsdl::readTypes()
{
    xmlReader.readNext();
    xmlReader.readNext();
    QString tempName = xmlReader.name().toString();

    if (xmlReader.name().toString() == QString::fromLatin1("schema")) {
        xmlReader.readNext();
    }
    else {
        enterErrorState(QString::fromLatin1("Error: file does not have WSDL schema tag inside!"));
        return;
    }

    QString lastName;
    tempName = QString::fromLatin1("");
    while(!xmlReader.atEnd()) {
        if (tempName != QString::fromLatin1(""))
            lastName = tempName;
        tempName = xmlReader.name().toString();

        if (tempName == QString::fromLatin1("element") && (xmlReader.attributes().count() == 1)) {
            QString elementName = xmlReader.attributes().value(QString::fromLatin1("name")).toString();
            workMethodList->append(elementName);
            readTypeSchemaElement();
        }
        else if (xmlReader.isEndElement() && (tempName == QString::fromLatin1("schema"))) {
            xmlReader.readNext();
            break;
        }
        else {
            xmlReader.readNext();
        }
    }
}

/*!
    \internal
  */
void QWsdl::readTypeSchemaElement()
{
    xmlReader.readNext();
    QMap<QString, QVariant> params;

    bool firstElem = true;
    QString tempName; //xmlReader.name().toString();
    QString lastName;
    while(!xmlReader.atEnd()) {
        if (tempName != QString::fromLatin1(""))
            lastName = tempName;
        tempName = xmlReader.name().toString();

        if (xmlReader.isEndElement()
                && (xmlReader.name() == QString::fromLatin1("element"))
                && ((lastName == QString::fromLatin1("complexType")) || (lastName == QString::fromLatin1("sequence")))
                && (firstElem == false)) {
            int currentMethodIndex = workMethodList->length() - 1;
            workMethodParameters->insert(currentMethodIndex, params);
            xmlReader.readNext();
            break;
        }

        if ((tempName == QString::fromLatin1("complexType")) || (tempName == QString::fromLatin1("sequence"))) {
            firstElem = false;
            xmlReader.readNext();
            continue;
        }

        if (tempName == QString::fromLatin1("element")) {
            firstElem = false;
            // Min and max occurences are not taken into account!
            QString elementName = xmlReader.attributes().value(QString::fromLatin1("name")).toString();
            // Ommits namespace ("s:int" => "int")
            QString elementType = xmlReader.attributes().value(QString::fromLatin1("type")).toString();

            if ((elementName == QString::fromLatin1("")) || (elementType == QString::fromLatin1(""))) {
                xmlReader.readNext();
                continue;
            }

            QVariant element;
            elementType = elementType.split(QChar::fromAscii(':')).at(1); // The int(1) looks very bad.
            // NEEDS MANY MORE VALUE TYPES! VERY SHAKY IMPLEMENTATION!
            // Prob'ly better to use schemas.
            if (elementType == QString::fromLatin1("int")) {
                element.setValue(int());
            }
            else if (elementType == QString::fromLatin1("float")) {
                element.setValue(float());
            }
            else if (elementType == QString::fromLatin1("double")) {
                element.setValue(double());
            }
            else if (elementType == QString::fromLatin1("boolean")) {
                element.setValue(true);
            }
            else if (elementType == QString::fromLatin1("dateTime")) {
                element.setValue(QDateTime());
            }
            else if (elementType == QString::fromLatin1("string")) {
                element.setValue(QString::fromLatin1(""));
            }
            else if (elementType == QString::fromLatin1("char")) {
                element.setValue(QChar());
            }
            else if (elementType.startsWith(QString::fromLatin1("ArrayOf"))) {
                elementType = elementType.mid(7);

                if (elementType == QString::fromLatin1("String")) {
                    element.setValue(QStringList());
                }
                else {//if (elementType == "DateTime") {
                    element.setValue(QList<QVariant>());
                }
            }
            else {
                element.setValue(QString::fromLatin1("temp"));
            }
            // VERY SHAKY IMPLEMENTATION!

            params.insert(elementName, element);
            xmlReader.readNext();
        }
        else {
            xmlReader.readNext();
        }
    }
}

/*!
    \internal

    Analyses both "working" QList and QMap, and extracts methods data, which is then put into
    'methods' QMap.
 */
void QWsdl::prepareMethods()
{
    if (errorState)
        return;

    bool methodsDone[workMethodList->length()];

    for (int x = 0; x < workMethodList->length(); x++)
        methodsDone[x] = false;


    for (int i = 0; (i < workMethodList->length()); i++) {
        if (methodsDone[i] == false) {
            bool isMethodAndResponsePresent = false;
            int methodMain = 0;
            int methodReturn = 0;
            QString methodName = workMethodList->at(i);

            if (methodName.contains(QString::fromLatin1("Response"))) {
                methodReturn = i;
                methodsDone[i] = true;
                QString tempMethodName = methodName;
                tempMethodName.chop(8);

                for (int j = 0; j < workMethodList->length(); j++) {
                    if (workMethodList->at(j) == tempMethodName) {
                        methodMain = j;
                        methodsDone[j] = true;
                        isMethodAndResponsePresent = true;
                        methodName = tempMethodName;
                        break;
                    }
                }
            }
            else {
                methodMain = i;

                for (int j = 0; j < workMethodList->length(); j++) {
                    if (workMethodList->at(j) == QString(methodName + "Response")) {
                        methodReturn = j;
                        methodsDone[j] = true;
                        isMethodAndResponsePresent = true;
                        break;
                    }
                }
            }

            if (isMethodAndResponsePresent == true) {
                QString methodPath;
                if (m_hostUrl.isEmpty())
                    methodPath = wsdlFilePath;
                else
                    methodPath = m_hostUrl.path();

                methodsMap->insert(methodName, new QWebServiceMethod(methodPath,
                                                             methodName,
                                                             m_targetNamespace,
                                                             workMethodParameters->value(methodMain)));
                methodsMap->value(methodName)->setReturnValue(workMethodParameters->value(methodReturn));
            }
        }
    }
}

/*!
    \internal
  */
void QWsdl::readMessages()
{
//    qDebug() << "WSDL :messages tag not supported yet.";
    xmlReader.readNext();
}

/*!
    \internal
  */
void QWsdl::readPorts()
{
//    qDebug() << "WSDL :portType tag not supported yet.";
    xmlReader.readNext();
}

/*!
    \internal
  */
void QWsdl::readBindings()
{
//    qDebug() << "WSDL :binding tag not supported yet.";
    xmlReader.readNext();
}

/*!
    \internal
  */
void QWsdl::readService()
{
    // TODO: add different addresses for different message types.
//    qDebug() << "WSDL :service tag not supported yet.";
    QString tempName;

    while (!xmlReader.atEnd()) {
        tempName = xmlReader.name().toString();

        if ((xmlReader.isEndElement()) && (tempName == QString::fromLatin1("service"))) {
            xmlReader.readNext();
            return;
        }

        if ((tempName == QString::fromLatin1("service"))
                && xmlReader.attributes().hasAttribute(QString::fromLatin1("name"))
                && (m_webServiceName == QString::fromLatin1(""))) {
            m_webServiceName = xmlReader.attributes().value(QString::fromLatin1("name")).toString();
        }

        if ((tempName == QString::fromLatin1("address"))
                && xmlReader.attributes().hasAttribute(QString::fromLatin1("location"))) {
            m_hostUrl.setUrl(xmlReader.attributes().value(QString::fromLatin1("location")).toString());
        }

        xmlReader.readNext();
    }
}

/*!
    \internal
  */
void QWsdl::readDocumentation()
{
//    qDebug() << "WSDL :documentation tag not supported yet.";
    xmlReader.readNext();
}

/*!
    \internal
  */
QString QWsdl::convertReplyToUtf(QString textToConvert)
{
    QString result = textToConvert;

    result.replace(QString::fromLatin1("&lt;"), QString::fromLatin1("<"));
    result.replace(QString::fromLatin1("&gt;"), QString::fromLatin1(">"));

    return result;
}
