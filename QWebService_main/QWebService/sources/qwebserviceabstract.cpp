#include "../headers/qwebserviceabstract.h"

QWebServiceAbstract::QWebServiceAbstract(QObject *parent)
    : QObject(parent)
{
}

QWebServiceAbstract::QWebServiceAbstract(QWsdl wsdl, QObject *parent)
    : QObject(parent)
{

}

QWebServiceAbstract::QWebServiceAbstract(QString hostname, QObject *parent)
    : QObject(parent)
{

}

QStringList QWebServiceAbstract::getMethodNames()
{

}

void QWebServiceAbstract::setHost(QString hostname)
{

}

void QWebServiceAbstract::setHost(QUrl hostUrl)
{

}

QUrl QWebServiceAbstract::getHostUrl()
{

}

QString QWebServiceAbstract::getHost()
{

}

QString QWebServiceAbstract::getWsdl()
{

}
