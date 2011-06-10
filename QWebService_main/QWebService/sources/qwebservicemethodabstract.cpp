#include "../headers/qwebservicemethodabstract.h"

QWebServiceMethodAbstract::QWebServiceMethodAbstract(QObject *parent) :
    QObject(parent)
{
}

//QWebServiceMethodAbstract::QWebServiceMethodAbstract(QString _hostname, QObject *parent) :
//    QObject(parent), hostname(_hostname)
//{

//}

QWebServiceMethodAbstract::QWebServiceMethodAbstract(QSoapMessage *msg, QObject *parent) :
    QObject(parent), message(msg)
{

}

QWebServiceMethodAbstract::~QWebServiceMethodAbstract()
{
    delete message;
}

//virtual QWebServiceMethodAbstract::QWebServiceMethodAbstract(QWsdl wsdl, QObject *parent);
/*
bool QWebServiceMethodAbstract::sendMessage(QList<QVariant> params)
{

}

bool QWebServiceMethodAbstract::sendMessage()
{

}
*/
void QWebServiceMethodAbstract::setHost(QString hostname)
{

}

void QWebServiceMethodAbstract::setHost(QUrl hostUrl)
{

}

QUrl QWebServiceMethodAbstract::getHostUrl()
{
    return hostUrl;
}

QString QWebServiceMethodAbstract::getHost()
{
    return hostname;
}

//QString QWebServiceMethodAbstract::getWsdl();
