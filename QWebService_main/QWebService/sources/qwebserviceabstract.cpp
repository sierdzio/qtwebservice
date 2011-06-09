#include "../headers/qwebserviceabstract.h"

QWebServiceAbstract::QWebServiceAbstract(QObject *parent)
    : QObject(parent)
{
    wsdl = new QWsdl(this);
}

QWebServiceAbstract::QWebServiceAbstract(QWsdl _wsdl, QObject *parent)
    : QObject(parent)
{
    this->wsdl = &_wsdl;
}

QWebServiceAbstract::QWebServiceAbstract(QString _hostname, QObject *parent)
    : QObject(parent), hostname(_hostname)
{
    hostUrl.setUrl(hostname);
    wsdl = new QWsdl(hostname, this);
}

QStringList QWebServiceAbstract::getMethodNames()
{
    wsdl->getMethodNames();
}

void QWebServiceAbstract::setHost(QString hostname)
{
    this->hostname = hostname;
    hostUrl.setUrl(hostname);
    wsdl->resetWsdl(hostname);
}

void QWebServiceAbstract::setHost(QUrl hostUrl)
{
    this->hostUrl = hostUrl;
    hostname = hostUrl.toString();
    wsdl->resetWsdl(hostname);
}

QUrl QWebServiceAbstract::getHostUrl()
{
    return hostUrl;
}

QString QWebServiceAbstract::getHost()
{
    return hostname;
}

//QString QWebServiceAbstract::getWsdl()
//{

//}
