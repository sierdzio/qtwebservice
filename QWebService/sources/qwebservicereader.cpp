#include "../headers/qwebservicereader.h"

/*!
    \class QWebServiceReader
    \brief A QWebServiceAbstract derivative aimed specifically at communicatin
           with external web services.

    This class is in a very early stage of development and should be used
    with great caution, if at all.
  */

/*!
    \fn QWebServiceReader::QWebServiceReader(QObject *parent)

    Constructs QWebServiceAbstract with \a parent, defauling to 0.
  */
QWebServiceReader::QWebServiceReader(QObject *parent) :
    QWebService(parent)
{
}

/*!
    \fn QWebServiceReader::QWebServiceReader(QWsdl *wsdl, QObject *parent)

    Constructs QWebServiceAbstract with \a wsdl and \a parent
    (which defaults to 0).
  */
QWebServiceReader::QWebServiceReader(QWsdl *wsdl, QObject *parent) :
    QWebService(wsdl, parent)
{
}

/*!
    \fn QWebServiceReader::QWebServiceReader(QString hostname, QObject *parent)

    Constructs QWebServiceAbstract with \a parent and \a hostname
    (which is used to create a wsdl object).
  */
QWebServiceReader::QWebServiceReader(QString hostname, QObject *parent) :
    QWebService(new QWsdl(hostname, parent), parent)
{

}

/*!
    \fn QWebServiceReader::methodHandler(QVariant param)

    Pure virtual method, can be used to handle methods. Probably a bad idea,
    might be dropped in the future.
    Accepts parameters in a QVariant \a param.
  */
void QWebServiceReader::methodHandler(QVariant param)
{
    Q_UNUSED(param)
}
