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
    wsdl = _wsdl;
    init();
}

/*!
    \fn QWebService::QWebService(QString _hostname, QObject *parent)

    Constructs and initializes the object using \a parent (defaults to 0).
    \a _hostname is used to initialise QWsdl.
  */
QWebService::QWebService(QString _hostname, QObject *parent)
    : QObject(parent)
{
    m_hostUrl.setUrl(_hostname);
    wsdl = new QWsdl(_hostname, this);
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
    \fn QWebService::errorEncountered(QString errMessage)

    Singal emitted when WsdlConverter encounters an error. Carries \a errMessage for convenience.
  */

/*!
    \fn QWebService::methodNames() const

    Returns a list of methods' names.
  */
QStringList QWebService::methodNames() const
{
    return (QStringList) messages->keys(); //wsdl->getMethodNames();
}

/*!
    \fn QWebService::methodParameters(QString methodName) const

    Returns list of names of parameters of a method specified by \a methodName.
  */
QStringList QWebService::methodParameters(QString methodName) const
{
    return messages->value(methodName)->parameterNames();
}

/*!
    \fn QWebService::methodReturnValue(QString methodName) const

    Returns names of return values, for a given method (\a methodName).
  */
QStringList QWebService::methodReturnValue(QString methodName) const
{
    return messages->value(methodName)->returnValueName();
}

/*!
    \fn QWebService::parameterNamesTypes(QString methodName) const

    For a given \a methodName, returns a QMap with QString paramater name as a key, and QVariant value.
    By running QVariant::typeName() you can determine the type of the parameter.
  */
QMap<QString, QVariant> QWebService::parameterNamesTypes(QString methodName) const
{
    return messages->value(methodName)->parameterNamesTypes();
}

/*!
    \fn QWebService::returnValueNameType(QString methodName) const

    For a given \a methodName, returns a QMap with QString paramater name as a key, and QVariant value.
    By running QVariant::typeName() you can determine the type of the parameter.
  */
QMap<QString, QVariant> QWebService::returnValueNameType(QString methodName) const
{
    return messages->value(methodName)->returnValueNameType();
}

/*!
    \fn QWebService::setHost(QString hostname)

    Sets new \a hostname URL, resets the WSDL, and reinitialises.
  */
void QWebService::setHost(QString hostname)
{
    m_hostUrl.setUrl(hostname);
    wsdl->resetWsdl(hostname);
    init();
}

/*!
    \fn QWebService::setHost(QUrl hostUrl)

    Sets new \a hostUrl URL, resets the WSDL, and reinitialises.
  */
void QWebService::setHost(QUrl hostUrl)
{
    this->m_hostUrl = hostUrl;
    wsdl->resetWsdl(hostUrl.host());
    init();
}

/*!
    \fn QWebService::hostUrl() const

    Returns QUrl of the web service.
  */
QUrl QWebService::hostUrl() const
{
    return m_hostUrl;
}

/*!
    \fn QWebService::host() const

    Returns QString with URL of the web service.
  */
QString QWebService::host() const
{
    return m_hostUrl.path();
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

/*!
    \fn QWebService::errorInfo() const

    Returns QString with error message in case an error occured. Otherwise, returns empty string.

    \sa isErrorState()
  */
QString QWebService::errorInfo() const
{
    return errorMessage;
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
    errorMessage = "";
    messages = new QMap<QString, QWebServiceMethod *>();

    if (isErrorState())
        return;

    messages = wsdl->methods();
}

/*!
    \internal
    \fn QWebService::enterErrorState(QString errMessage)

    Enters into error state with message \a errMessage.
  */
bool QWebService::enterErrorState(QString errMessage)
{
    errorState = true;
    errorMessage += errMessage + " ";
    qDebug() << errMessage;
    emit errorEncountered(errMessage);
    return false;
}
