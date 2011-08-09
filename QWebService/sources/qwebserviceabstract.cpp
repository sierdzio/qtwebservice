#include "../headers/qwebserviceabstract.h"

/*!
    \class QWebServiceAbstract
    \brief Parent Web Service class.

    Currently acts mostly as QWsdl wrapper.
  */

/*!
    \fn QWebServiceAbstract::QWebServiceAbstract(QObject *parent)

    Constructs the object with empty QWsdl and using \a parent (defaults to 0).
  */
QWebServiceAbstract::QWebServiceAbstract(QObject *parent)
    : QObject(parent)
{
    wsdl = new QWsdl(this);
    errorState = false;
    isErrorState();
}

/*!
    \fn QWebServiceAbstract::QWebServiceAbstract(QWsdl *_wsdl, QObject *parent)

    Constructs and initializes the object with \a _wsdl and using \a parent (defaults to 0).
  */
QWebServiceAbstract::QWebServiceAbstract(QWsdl *_wsdl, QObject *parent)
    : QObject(parent)
{
    this->wsdl = _wsdl;
    init();
}

/*!
    \fn QWebServiceAbstract::QWebServiceAbstract(QString _hostname, QObject *parent)

    Constructs and initializes the object using \a parent (defaults to 0).
    \a _hostname is used to initialise QWsdl.
  */
QWebServiceAbstract::QWebServiceAbstract(QString _hostname, QObject *parent)
    : QObject(parent), host(_hostname)
{
    hostUrl.setUrl(host);
    wsdl = new QWsdl(host, this);
    init();
}

/*!
    \fn QWebServiceAbstract::~QWebServiceAbstract()

    Deletes wsdl pointer.
  */
QWebServiceAbstract::~QWebServiceAbstract()
{
    delete wsdl;
}

/*!
    \fn QWebServiceAbstract::getMethodNames()

    Returns a list of methods' names.
  */
QStringList QWebServiceAbstract::getMethodNames()
{
    return (QStringList) messages->keys(); //wsdl->getMethodNames();
}

/*!
    \fn QWebServiceAbstract::getMethodParameters(QString methodName) const

    Returns list of names of parameters of a method specified by \a methodName.
  */
QStringList QWebServiceAbstract::getMethodParameters(QString methodName) const
{
    return messages->value(methodName)->getParameterNames();
}

/*!
    \fn QWebServiceAbstract::getMethodReturnValue(QString methodName) const

    Returns names of return values, for a given method (\a methodName).
  */
QStringList QWebServiceAbstract::getMethodReturnValue(QString methodName) const
{
    return messages->value(methodName)->getReturnValueName();
}

/*!
    \fn QWebServiceAbstract::getParameterNamesTypes(QString methodName) const

    For a given \a methodName, returns a QMap with QString paramater name as a key, and QVariant value.
    By running QVariant::typeName() you can determine the type of the parameter.
  */
QMap<QString, QVariant> QWebServiceAbstract::getParameterNamesTypes(QString methodName) const
{
    return messages->value(methodName)->getParameterNamesTypes();
}

/*!
    \fn QWebServiceAbstract::getReturnValueNameType(QString methodName) const

    For a given \a methodName, returns a QMap with QString paramater name as a key, and QVariant value.
    By running QVariant::typeName() you can determine the type of the parameter.
  */
QMap<QString, QVariant> QWebServiceAbstract::getReturnValueNameType(QString methodName) const
{
    return messages->value(methodName)->getReturnValueNameType();
}

/*!
    \fn QWebServiceAbstract::setHost(QString hostname)

    Sets new \a hostname URL, resets the WSDL, and reinitialises.
  */
void QWebServiceAbstract::setHost(QString hostname)
{
    this->host = hostname;
    hostUrl.setUrl(hostname);
    wsdl->resetWsdl(hostname);
    init();
}

/*!
    \fn QWebServiceAbstract::setHost(QUrl hostUrl)

    Sets new \a hostUrl URL, resets the WSDL, and reinitialises.
  */
void QWebServiceAbstract::setHost(QUrl hostUrl)
{
    this->hostUrl = hostUrl;
    host = hostUrl.toString();
    wsdl->resetWsdl(host);
    init();
}

/*!
    \fn QWebServiceAbstract::getHostUrl()

    Returns QUrl of the web service.
  */
QUrl QWebServiceAbstract::getHostUrl()
{
    return hostUrl;
}

/*!
    \fn QWebServiceAbstract::getHost()

    Returns QString with URL of the web service.
  */
QString QWebServiceAbstract::getHost()
{
    return host;
}

/*!
    \fn QWebServiceAbstract::isErrorState()

    Returns true if object is in error state.
  */
bool QWebServiceAbstract::isErrorState()
{
    if (wsdl->isErrorState())
        errorState = true;
    return errorState;
}

//QString QWebServiceAbstract::getWsdl()
//{

//}

/*!
    \internal
    \fn QWebServiceAbstract::init()

    Initialises the object, resets error state.
  */
void QWebServiceAbstract::init()
{
    errorState = false;
    messages = new QMap<QString, QSoapMessage *>();

    if (isErrorState())
        return;

    messages = wsdl->getMethods();
}
