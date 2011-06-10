#include "../headers/qwebservicereaderabstract.h"

QWebServiceReaderAbstract::QWebServiceReaderAbstract(QObject *parent) :
    QWebServiceAbstract(parent)
{
}

QWebServiceReaderAbstract::QWebServiceReaderAbstract(QWsdl *wsdl, QObject *parent) :
    QWebServiceAbstract(wsdl, parent)
{

}

QWebServiceReaderAbstract::QWebServiceReaderAbstract(QString hostname, QObject *parent) :
    QWebServiceAbstract(new QWsdl(hostname, parent), parent)
{

}

void QWebServiceReaderAbstract::methodHandler(QVariant param)
{

}
