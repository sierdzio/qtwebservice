#include "../headers/qwebservicemethodabstract.h"

QWebServiceMethodAbstract::QWebServiceMethodAbstract(QObject *parent) :
    QObject(parent)
{
}

QWebServiceMethodAbstract::QWebServiceMethodAbstract(QString hostname, QObject *parent)
{

}

//virtual QWebServiceMethodAbstract::QWebServiceMethodAbstract(QWsdl wsdl, QObject *parent);

bool QWebServiceMethodAbstract::sendMessage(QList<QVariant> params)
{

}

bool QWebServiceMethodAbstract::sendMessage()
{

}

void QWebServiceMethodAbstract::setHost(QString hostname)
{

}

void QWebServiceMethodAbstract::setHost(QUrl hostUrl)
{

}

QUrl QWebServiceMethodAbstract::getHostUrl()
{

}

QString QWebServiceMethodAbstract::getHost()
{

}

//QString QWebServiceMethodAbstract::getWsdl();
