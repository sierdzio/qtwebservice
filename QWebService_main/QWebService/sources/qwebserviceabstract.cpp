#include "../headers/qwebserviceabstract.h"

QWebServiceAbstract::QWebServiceAbstract(QObject *parent)
    : QObject(parent)
{
    wsdl = new QWsdl(this);
    errorState = false;
    isErrorState();
}

QWebServiceAbstract::QWebServiceAbstract(QWsdl *_wsdl, QObject *parent)
    : QObject(parent)
{
    this->wsdl = _wsdl;
    init();
}

QWebServiceAbstract::QWebServiceAbstract(QString _hostname, QObject *parent)
    : QObject(parent), hostname(_hostname)
{
    hostUrl.setUrl(hostname);
    wsdl = new QWsdl(hostname, this);
    init();
}

QWebServiceAbstract::~QWebServiceAbstract()
{
    delete wsdl;
}

QStringList QWebServiceAbstract::getMethodNames()
{
    return (QStringList) messages->keys(); //wsdl->getMethodNames();
}

QStringList QWebServiceAbstract::getMethodParameters(QString methodName) const
{
    return messages->value(methodName)->getParameterNames();
}

QStringList QWebServiceAbstract::getMethodReturnValue(QString methodName) const
{
    return messages->value(methodName)->getReturnValueName();
}

QMap<QString, QVariant> QWebServiceAbstract::getParameterNamesTypes(QString methodName) const
{
    return messages->value(methodName)->getParameterNamesTypes();
}

QMap<QString, QVariant> QWebServiceAbstract::getReturnValueNameType(QString methodName) const
{
    return messages->value(methodName)->getReturnValueNameType();
}

void QWebServiceAbstract::setHost(QString hostname)
{
    this->hostname = hostname;
    hostUrl.setUrl(hostname);
    wsdl->resetWsdl(hostname);
    init();
}

void QWebServiceAbstract::setHost(QUrl hostUrl)
{
    this->hostUrl = hostUrl;
    hostname = hostUrl.toString();
    wsdl->resetWsdl(hostname);
    init();
}

QUrl QWebServiceAbstract::getHostUrl()
{
    return hostUrl;
}

QString QWebServiceAbstract::getHost()
{
    return hostname;
}

bool QWebServiceAbstract::isErrorState()
{
    if (wsdl->isErrorState())
        errorState = true;
    return errorState;
}

//QString QWebServiceAbstract::getWsdl()
//{

//}

void QWebServiceAbstract::init()
{
    errorState = false;
    messages = new QMap<QString, QSoapMessage *>();

    if (isErrorState())
        return;

    messages = wsdl->getMethods();
}
