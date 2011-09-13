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
    You can set a WSDL file later using setWsdl(), or add custom web methods
    using addMethod().

    \sa setWsdl(), addMethod()
  */
QWebService::QWebService(QObject *parent)
    : QObject(parent)
{
    wsdl = new QWsdl(this);
    methods = new QMap<QString, QWebServiceMethod *>();
    init();
}

/*!
    \fn QWebService::QWebService(QWsdl *_wsdl, QObject *parent)

    Constructs and initializes the object with \a _wsdl and using \a parent (defaults to 0).
    WSDL file can be resetted later using resetWsdl(). You can add custom web methods
    using addMethod().

    \sa resetWsdl(), addMethod()
  */
QWebService::QWebService(QWsdl *_wsdl, QObject *parent)
    : QObject(parent)
{
    wsdl = _wsdl;
    methods = new QMap<QString, QWebServiceMethod *>();
    init();
    methods = wsdl->methods();
}

/*!
    \fn QWebService::QWebService(QString _hostname, QObject *parent)

    Constructs and initializes the object using \a parent (defaults to 0).
    \a _hostname is used to initialise QWsdl if an URL or path to a valid WSDL file
    was used. Otherwise, WSDL can be set later with setWsdl(), or add custom web methods
    using addMethod().

    \sa setWsdl(), addMethod()
  */
QWebService::QWebService(QString _hostname, QObject *parent)
    : QObject(parent)
{
    m_hostUrl.setUrl(_hostname);
    wsdl = new QWsdl(_hostname, this);
    methods = new QMap<QString, QWebServiceMethod *>();
    init();

    if (!wsdl->isErrorState())
        methods = wsdl->methods();
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
    return (QStringList) methods->keys(); //wsdl->getMethodNames();
}

/*!
    \fn QWebService::methodParameters(QString methodName) const

    Returns list of names of parameters of a method specified by \a methodName.
  */
QStringList QWebService::methodParameters(QString methodName) const
{
    return methods->value(methodName)->parameterNames();
}

/*!
    \fn QWebService::methodReturnValue(QString methodName) const

    Returns names of return values, for a given method (\a methodName).
  */
QStringList QWebService::methodReturnValue(QString methodName) const
{
    return methods->value(methodName)->returnValueName();
}

/*!
    \fn QWebService::parameterNamesTypes(QString methodName) const

    For a given \a methodName, returns a QMap with QString paramater name as a key, and QVariant value.
    By running QVariant::typeName() you can determine the type of the parameter.
  */
QMap<QString, QVariant> QWebService::parameterNamesTypes(QString methodName) const
{
    return methods->value(methodName)->parameterNamesTypes();
}

/*!
    \fn QWebService::returnValueNameType(QString methodName) const

    For a given \a methodName, returns a QMap with QString paramater name as a key, and QVariant value.
    By running QVariant::typeName() you can determine the type of the parameter.
  */
QMap<QString, QVariant> QWebService::returnValueNameType(QString methodName) const
{
    return methods->value(methodName)->returnValueNameType();
}

/*!
  \fn QWebService::addMethod(QWebServiceMethod *newMethod)

  Adds the specified web method (\a newMethod) to the web service.
  Method name should be set in the object, it is used internally and
  for referencing.

  \sa removeMethod()
  */
void QWebService::addMethod(QWebServiceMethod *newMethod)
{
    methods->insert(newMethod->metodName(), newMethod);
}

/*!
  \fn QWebService::addMethod(QString methodName, QWebServiceMethod *newMethod)
  \overload addMethod()

  Adds the specified web method (\a newMethod) to the web service.
  Method name (\methodName) is used for reference (it does not set name
  inside the QWebServiceMethod object).

  \sa removeMethod()
  */
void QWebService::addMethod(QString methodName, QWebServiceMethod *newMethod)
{
//    newMethod->setMessageName(methodName);
    methods->insert(methodName, newMethod);
}

/*!
  \fn QWebService::removeMethod(QString methodName)

  Removes the method specified by \a methodName.
  It also deletes the underlying object, so be careful!

  \sa addMethod()
  */
void QWebService::removeMethod(QString methodName)
{
    delete methods->value(methodName);
    methods->remove(methodName);
}

/*!
    \fn QWebService::setHost(QString hostname)

    Sets new \a hostname URL, resets the WSDL, and reinitialises.
  */
void QWebService::setHost(QString hostname)
{
    m_hostUrl.setUrl(hostname);
    resetWsdl(new QWsdl(hostname, this));
    init();
}

/*!
    \fn QWebService::setHost(QUrl hostUrl)
    \overload setHost()

    Sets new \a hostUrl URL, resets the WSDL, and reinitialises.
  */
void QWebService::setHost(QUrl hostUrl)
{
    m_hostUrl = hostUrl;
    resetWsdl(new QWsdl(hostUrl.host(), this));
    init();
}

/*!
  \fn QWebService::setWsdl(QWsdl newWsdl)

  Sets the WSDL (\a newWsdl) file to use. This does not override already present methods.
  If you want to override them, use resetWsdl().

  \sa resetWsdl()
  */
void QWebService::setWsdl(QWsdl *newWsdl)
{
//    delete wsdl;
    wsdl = newWsdl;
    foreach (QString s, wsdl->methods()->keys()) {
        methods->insert(s, wsdl->methods()->value(s));
    }
}

/*!
  \fn QWebService::resetWsdl(QWsdl newWsdl)

  Sets the WSDL (\a newWsdl) file to use. This does override already present methods. If you just want
  to add WSDL methods to existing ones, use setWsdl()

  \sa setWsdl()
  */
void QWebService::resetWsdl(QWsdl *newWsdl)
{
//    delete wsdl;

    if (newWsdl == 0) {
        methods->clear();
        wsdl = new QWsdl(this);
    }
    else {
        wsdl = newWsdl;
        methods->clear();
//        delete methods;
        methods = wsdl->methods();
    }
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
    errorMessage = QString::fromLatin1("");

    if (isErrorState())
        return;
}

/*!
    \internal
    \fn QWebService::enterErrorState(QString errMessage)

    Enters into error state with message \a errMessage.
  */
bool QWebService::enterErrorState(QString errMessage)
{
    errorState = true;
    errorMessage += QString(errMessage + " ");
//    qDebug() << errMessage;
    emit errorEncountered(errMessage);
    return false;
}
