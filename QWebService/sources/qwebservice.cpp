#include "../headers/qwebservice.h"

/*!
    \class QWebService
    \brief Parent Web Service class.

    Currently acts mostly as QWsdl wrapper. Some functionality might be pushed to
    QWebServiceReader in the future, to make this class more general.
  */

/*!
    \fn QWebService::QWebService(QObject *parent)

    Constructs the object with empty QWsdl and using \a parent (defaults to 0).
  */
QWebService::QWebService(QObject *parent)
    : QObject(parent)
{
    wsdl = new QWsdl(this);
    errorState = false;
    isErrorState();
}

/*!
    \fn QWebService::QWebService(QWsdl *_wsdl, QObject *parent)

    Constructs and initializes the object with \a _wsdl and using \a parent (defaults to 0).
  */
QWebService::QWebService(QWsdl *_wsdl, QObject *parent)
    : QObject(parent)
{
    this->wsdl = _wsdl;
    init();
}

/*!
    \fn QWebService::QWebService(QString _hostname, QObject *parent)

    Constructs and initializes the object using \a parent (defaults to 0).
    \a _hostname is used to initialise QWsdl.
  */
QWebService::QWebService(QString _hostname, QObject *parent)
    : QObject(parent), host(_hostname)
{
    hostUrl.setUrl(host);
    wsdl = new QWsdl(host, this);
    init();
}

/*!
    \fn QWebService::~QWebService()

    Deletes wsdl pointer.
  */
QWebService::~QWebService()
{
    delete wsdl;
}

/*!
    \fn QWebService::getMethodNames()

    Returns a list of methods' names.
  */
QStringList QWebService::getMethodNames()
{
    return (QStringList) messages->keys(); //wsdl->getMethodNames();
}

/*!
    \fn QWebService::getMethodParameters(QString methodName) const

    Returns list of names of parameters of a method specified by \a methodName.
  */
QStringList QWebService::getMethodParameters(QString methodName) const
{
    return messages->value(methodName)->getParameterNames();
}

/*!
    \fn QWebService::getMethodReturnValue(QString methodName) const

    Returns names of return values, for a given method (\a methodName).
  */
QStringList QWebService::getMethodReturnValue(QString methodName) const
{
    return messages->value(methodName)->getReturnValueName();
}

/*!
    \fn QWebService::getParameterNamesTypes(QString methodName) const

    For a given \a methodName, returns a QMap with QString paramater name as a key, and QVariant value.
    By running QVariant::typeName() you can determine the type of the parameter.
  */
QMap<QString, QVariant> QWebService::getParameterNamesTypes(QString methodName) const
{
    return messages->value(methodName)->getParameterNamesTypes();
}

/*!
    \fn QWebService::getReturnValueNameType(QString methodName) const

    For a given \a methodName, returns a QMap with QString paramater name as a key, and QVariant value.
    By running QVariant::typeName() you can determine the type of the parameter.
  */
QMap<QString, QVariant> QWebService::getReturnValueNameType(QString methodName) const
{
    return messages->value(methodName)->getReturnValueNameType();
}

/*!
    \fn QWebService::setHost(QString hostname)

    Sets new \a hostname URL, resets the WSDL, and reinitialises.
  */
void QWebService::setHost(QString hostname)
{
    this->host = hostname;
    hostUrl.setUrl(hostname);
    wsdl->resetWsdl(hostname);
    init();
}

/*!
    \fn QWebService::setHost(QUrl hostUrl)

    Sets new \a hostUrl URL, resets the WSDL, and reinitialises.
  */
void QWebService::setHost(QUrl hostUrl)
{
    this->hostUrl = hostUrl;
    host = hostUrl.toString();
    wsdl->resetWsdl(host);
    init();
}

/*!
    \fn QWebService::getHostUrl()

    Returns QUrl of the web service.
  */
QUrl QWebService::getHostUrl()
{
    return hostUrl;
}

/*!
    \fn QWebService::getHost()

    Returns QString with URL of the web service.
  */
QString QWebService::getHost()
{
    return host;
}

/*!
    \fn QWebService::isErrorState()

    Returns true if object is in error state.
  */
bool QWebService::isErrorState()
{
    if (wsdl->isErrorState())
        errorState = true;
    return errorState;
}

//QString QWebService::getWsdl()
//{

//}

/*!
    \internal
    \fn QWebService::init()

    Initialises the object, resets error state.
  */
void QWebService::init()
{
    errorState = false;
    messages = new QMap<QString, QWebMethod *>();

    if (isErrorState())
        return;

    messages = wsdl->getMethods();
}
