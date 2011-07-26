#include "../headers/qwebservicereaderabstract.h"

/*!
    \class QWebServiceReaderAbstract
    \brief A QWebServiceAbstract derivative aimed specifically at communicatin with external web services.

    This class is in a very early stage of development and should be used with great caution, if at all.
  */

/*!
    \fn QWebServiceReaderAbstract::QWebServiceReaderAbstract(QObject *parent)

    Constructs QWebServiceAbstract with \a parent, defauling to 0.
  */
QWebServiceReaderAbstract::QWebServiceReaderAbstract(QObject *parent) :
    QWebServiceAbstract(parent)
{
}

/*!
    \fn QWebServiceReaderAbstract::QWebServiceReaderAbstract(QWsdl *wsdl, QObject *parent)

    Constructs QWebServiceAbstract with \a wsdl and \a parent (which defaults to 0).
  */
QWebServiceReaderAbstract::QWebServiceReaderAbstract(QWsdl *wsdl, QObject *parent) :
    QWebServiceAbstract(wsdl, parent)
{
}

/*!
    \fn QWebServiceReaderAbstract::QWebServiceReaderAbstract(QString hostname, QObject *parent)

    Constructs QWebServiceAbstract with \a parent and \a hostname (which is used to create a wsdl object).
  */
QWebServiceReaderAbstract::QWebServiceReaderAbstract(QString hostname, QObject *parent) :
    QWebServiceAbstract(new QWsdl(hostname, parent), parent)
{

}

/*!
    \fn QWebServiceReaderAbstract::methodHandler(QVariant param)

    Pure virtual method, can be used to handle methods. Probably a bad idea, might be dropped in the future.
    Accepts parameters in a QVariant \a param.
  */
void QWebServiceReaderAbstract::methodHandler(QVariant param)
{
}
