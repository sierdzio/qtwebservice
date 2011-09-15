/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the QWebService library, QtNetwork Module.
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

#include "../headers/qwsdl.h"

/*!
    \class QWsdl
    \brief Class for interaction with local and remote WSDL files.
           Currently read-only.

    Reads web service data (message names, parameters, return values,
    ws name etc) from a WSDL file or URL.
  */

/*!
    Simple constructor, requires \a parent only, but needs other information
    to be specified later in order to run.
    You need to run setWsdlFile(), which automatically parses the file.

    Initialises the whole object with default values.

    \sa setWsdlFile(), resetWsdl()
  */
QWsdl::QWsdl(QObject *parent) :
    QObject(parent)
{
//    wsdlFilePath = QString();
    init();
}

/*!
    Constructs the object using optional \a parent. Uses the file path or URL
    specified in \a wsdlFile to parse the WSDL file.
  */
QWsdl::QWsdl(const QString &wsdlFile, QObject *parent) :
    QObject(parent), wsdlFilePath(wsdlFile)
{
    init();
    parse();
}

/*!
    Destructor - cleans some internal variables.
  */
QWsdl::~QWsdl()
{
    delete methodsMap;
    delete workMethodList;
    delete workMethodParameters;
}

/*!
    \fn QWsdl::errorEncountered(const QString &errMessage)

    Singal emitted when WsdlConverter encounters an error.
    Carries \a errMessage for convenience.
  */

/*!
    Wrapper for resetWsdl(). Used to set the WSDL file or URL using \a wsdlFile.
    Compulsory after simple constructor, but not needed if you have already
    specified the file in the constructor or resetWsdl().

    \sa resetWsdl()
  */
void QWsdl::setWsdlFile(const QString &wsdlFile) // == resetWsdl()
{
    resetWsdl(wsdlFile);
}

/*!
    Can be used to set or reset a WSDL file (or URL), using \a newWsdl.
    Cleans and reinitialises the object, parses the file.

    \sa setWsdlFile()
  */
void QWsdl::resetWsdl(const QString &newWsdl)
{
    wsdlFilePath = newWsdl;

    methodsMap->clear();
    workMethodList->clear();
    workMethodParameters->clear();
    replyReceived = false;
    errorState = false;
    errorMessage = QLatin1String("");
    m_webServiceName = QLatin1String("");
    m_hostUrl.setUrl(QLatin1String(""));
    m_targetNamespace = QLatin1String("");
    xmlReader.clear();

    parse();
}

/*!
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
    Returns a QMap<QString, QWebServiceMethod *> pointer.
    Keys are method names (just as in getMethodNames()), and values are
    QWebServiceMethods themselves (which means they can be used
    not only to get information, but also to send messages, set them up etc.).

    \sa methodNames()
  */
QMap<QString, QWebServiceMethod *> *QWsdl::methods()
{
    return methodsMap;
}

/*!
    Returns QString with the name of the web service specified in WSDL.
  */
QString QWsdl::webServiceName() const
{
    return m_webServiceName;
}

/*!
    Returns web service's URL. If there is no valid URL in WSDL file,
    path to this file is returned.

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
    Returns web service's URL. If there is no valid URL in WSDL file,
    path to this file is returned.

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
    Returns path to WSDL file.
  */
QString QWsdl::wsdlFile() const
{
    return wsdlFilePath;
}

/*!
    Returns target namespace specified in WSDL.
  */
QString QWsdl::targetNamespace() const
{
    return m_targetNamespace;
}

/*!
    Returns QString with error message in case an error occured.
    Otherwise, returns empty string.

    \sa isErrorState()
  */
QString QWsdl::errorInfo() const
{
    return errorMessage;
}

/*!
    Returns true if there was an error, false otherwise.
    Details about an error can be read with getErrorInfo().

    \sa errorInfo()
  */
bool QWsdl::isErrorState() const
{
    return errorState;
}

/*!
    Asynchronous public return slot. Reads WSDL reply (\a rply)
    from server (used in case URL was specified in wsdl file path).

    Not sure why it's public (sierdzio). Maybe it will go private in the future.
  */
void QWsdl::fileReplyFinished(QNetworkReply *rply)
{
    QString replyString = convertReplyToUtf(QLatin1String(rply->readAll()));
    QFile file(QLatin1String("tempWsdl.asmx~"));

    if (file.exists())
        file.remove();

    if (!file.open(QFile::WriteOnly)) {
        enterErrorState(
                    QString("Error: cannot write WSDL file from remote location. Reason: "
                            + file.errorString()));
        return;
    } else {
        file.write(replyString.toUtf8());
    }

    file.close();
    replyReceived = true;
    rply->deleteLater();
//    emit replyReady(reply);
}

/*!
    \internal

    Initialises the object.
  */
void QWsdl::init()
{
    replyReceived = false;
    errorState = false;
    errorMessage = QLatin1String("");
    m_webServiceName = QLatin1String("");
    m_hostUrl.setUrl(QLatin1String(""));
    m_targetNamespace = QLatin1String("");

    workMethodList = new QStringList();
    workMethodParameters = new QMap<int, QMap<QString, QVariant> >();
    methodsMap = new QMap<QString, QWebServiceMethod *>;
}

/*!
    \internal

    Enters into error state with message \a errMessage.
  */
bool QWsdl::enterErrorState(const QString &errMessage)
{
    errorState = true;
    errorMessage += QString(errMessage + " ");
//    qDebug() << errMessage;
    emit errorEncountered(errMessage);
    return false;
}

/*!
    \internal

    Central method of this class. Parses the WSDL file, creates all
    QWebServiceMethods, reads all necessary data,
    like web service's name etc.
  */
bool QWsdl::parse()
{
    /*
      Algorithm extracts method names from "types" tags,
      which is most probably wrong, as it should be
      cross-checked with other tags ("message", etc.)
    */

    if (errorState) {
        enterErrorState(QLatin1String("WSDL reader is in error state "
                                            "and cannot parse the file."));
        return false;
    }

    prepareFile();

    QFile file(wsdlFilePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        enterErrorState(QString("Error: cannot read WSDL file: "
                                + wsdlFilePath + ". Reason: " + file.errorString()));
        return false;
    }

    xmlReader.setDevice(&file);
    xmlReader.readNext();

    while (!xmlReader.atEnd()) {
        if (xmlReader.isStartElement()) {
            QString tempN = xmlReader.name().toString();

            if (tempN == QLatin1String("definitions")) {
                m_targetNamespace = xmlReader.attributes().value(
                            QLatin1String("targetNamespace")).toString();
                readDefinitions();
            } else {
                enterErrorState(QLatin1String("Error: file does not have "
                                                    "WSDL definitions inside!"));
                return false;
            }
        } else {
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

    If the host path is not a local file, but URL, QWsdl will download
    it into a temporary file, then read, and delete at exit.
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
    tagUsed.insert(QLatin1String("types"), false);
    tagUsed.insert(QLatin1String("message"), false);
    tagUsed.insert(QLatin1String("portType"), false);
    tagUsed.insert(QLatin1String("binding"), false);
    tagUsed.insert(QLatin1String("service"), false);
    tagUsed.insert(QLatin1String("documentation"), false);
    //END of EXPERIMENTAL

    xmlReader.readNext();
    QString tempName = xmlReader.name().toString();

    while(!xmlReader.atEnd()) {
        tempName = xmlReader.name().toString();

        if (xmlReader.isEndElement()
                && (tempName == QLatin1String("definitions"))) {
            xmlReader.readNext();
            break;
        } else if (xmlReader.isEndElement()) {
            xmlReader.readNext();
            continue;
        }

        if (tempName == QLatin1String("types")) {
            readTypes();
//            tagUsed.insert("types", true);
            xmlReader.readNext();
        } else if (tempName == QLatin1String("message")) {
            if (!tagUsed.value(QLatin1String("message")))
                readMessages();
            tagUsed.insert(QLatin1String("message"), true);
            xmlReader.readNext();
        } else if (tempName == QLatin1String("portType")) {
            if (!tagUsed.value(QLatin1String("portType")))
                readPorts();
            tagUsed.insert(QLatin1String("portType"), true);
            xmlReader.readNext();
        } else if (tempName == QLatin1String("binding")) {
            if (!tagUsed.value(QLatin1String("binding")))
                readBindings();
            tagUsed.insert(QLatin1String("binding"), true);
            xmlReader.readNext();
        } else if (tempName == QLatin1String("service")) {
//            if (!tagUsed.value("service"))
                readService();
            tagUsed.insert(QLatin1String("service"), true);
            xmlReader.readNext();
        } else if (tempName == QLatin1String("documentation")) {
            if (!tagUsed.value(QLatin1String("documentation")))
                readDocumentation();
            tagUsed.insert(QLatin1String("documentation"), true);
            xmlReader.readNext();
        } else {
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

    if (xmlReader.name().toString() == QLatin1String("schema")) {
        xmlReader.readNext();
    } else {
        enterErrorState(QLatin1String("Error: file does not have WSDL "
                                            "schema tag inside!"));
        return;
    }

    QString lastName;
    tempName = QLatin1String("");
    while(!xmlReader.atEnd()) {
        if (tempName != QLatin1String(""))
            lastName = tempName;
        tempName = xmlReader.name().toString();

        if (tempName == QLatin1String("element")
                && (xmlReader.attributes().count() == 1)) {
            QString elementName = xmlReader.attributes().value(
                        QLatin1String("name")).toString();
            workMethodList->append(elementName);
            readTypeSchemaElement();
        } else if (xmlReader.isEndElement()
                 && (tempName == QLatin1String("schema"))) {
            xmlReader.readNext();
            break;
        } else {
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
        if (tempName != QLatin1String(""))
            lastName = tempName;
        tempName = xmlReader.name().toString();

        if (xmlReader.isEndElement()
                && (xmlReader.name() == QLatin1String("element"))
                && ((lastName == QLatin1String("complexType"))
                    || (lastName == QLatin1String("sequence")))
                && (firstElem == false)) {
            int currentMethodIndex = workMethodList->length() - 1;
            workMethodParameters->insert(currentMethodIndex, params);
            xmlReader.readNext();
            break;
        }

        if ((tempName == QLatin1String("complexType"))
                || (tempName == QLatin1String("sequence"))) {
            firstElem = false;
            xmlReader.readNext();
            continue;
        }

        if (tempName == QLatin1String("element")) {
            firstElem = false;
            // Min and max occurences are not taken into account!
            QString elementName = xmlReader.attributes().value(
                        QLatin1String("name")).toString();
            // Ommits namespace ("s:int" => "int")
            QString elementType = xmlReader.attributes().value(
                        QLatin1String("type")).toString();

            if ((elementName == QLatin1String(""))
                    || (elementType == QLatin1String(""))) {
                xmlReader.readNext();
                continue;
            }

            QVariant element;
            // The int(1) looks very bad.
            elementType = elementType.split(QChar::fromAscii(':')).at(1);
            // NEEDS MANY MORE VALUE TYPES! VERY SHAKY IMPLEMENTATION!
            // Prob'ly better to use schemas.
            if (elementType == QLatin1String("int")) {
                element.setValue(int());
            } else if (elementType == QLatin1String("float")) {
                element.setValue(float());
            } else if (elementType == QLatin1String("double")) {
                element.setValue(double());
            } else if (elementType == QLatin1String("boolean")) {
                element.setValue(true);
            } else if (elementType == QLatin1String("dateTime")) {
                element.setValue(QDateTime());
            } else if (elementType == QLatin1String("string")) {
                element.setValue(QString());
            } else if (elementType == QLatin1String("char")) {
                element.setValue(QChar());
            } else if (elementType.startsWith(QLatin1String("ArrayOf"))) {
                elementType = elementType.mid(7);

                if (elementType == QLatin1String("String")) {
                    element.setValue(QStringList());
                } else {//if (elementType == "DateTime") {
                    element.setValue(QList<QVariant>());
                }
            } else {
                element.setValue(QString());
            }
            // VERY SHAKY IMPLEMENTATION!

            params.insert(elementName, element);
            xmlReader.readNext();
        } else {
            xmlReader.readNext();
        }
    }
}

/*!
    \internal

    Analyses both "working" QList and QMap, and extracts methods data,
    which is then put into 'methods' QMap.
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

            if (methodName.contains(QLatin1String("Response"))) {
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
            } else {
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

                methodsMap->insert(methodName,
                                   new QWebServiceMethod(methodPath,
                                                         methodName,
                                                         m_targetNamespace,
                                                         workMethodParameters->value(methodMain)));
                methodsMap->value(methodName)->setReturnValue(
                            workMethodParameters->value(methodReturn));
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

        if ((xmlReader.isEndElement())
                && (tempName == QLatin1String("service"))) {
            xmlReader.readNext();
            return;
        }

        if ((tempName == QLatin1String("service"))
                && xmlReader.attributes().hasAttribute(
                    QLatin1String("name"))
                && (m_webServiceName == QLatin1String(""))) {
            m_webServiceName = xmlReader.attributes().value(
                        QLatin1String("name")).toString();
        }

        if ((tempName == QLatin1String("address"))
                && xmlReader.attributes().hasAttribute(
                    QLatin1String("location"))) {
            m_hostUrl.setUrl(xmlReader.attributes().value(
                                 QLatin1String("location")).toString());
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
QString QWsdl::convertReplyToUtf(const QString &textToConvert)
{
    QString result = textToConvert;

    result.replace(QLatin1String("&lt;"), QLatin1String("<"));
    result.replace(QLatin1String("&gt;"), QLatin1String(">"));

    return result;
}
