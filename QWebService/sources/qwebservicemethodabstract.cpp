#include "../headers/qwebservicemethodabstract.h"

/*!
    \class QWebServiceMethodAbstract
    \brief Unused class, was meant to be a high-level wrapper for QSoapMessage.

    This class will probably be dropped, or at least seriously refactored in the future.
  */

/*!
    \fn QWebServiceMethodAbstract::QWebServiceMethodAbstract(QObject *parent)

    Pretty much the default constructor, passes \a parent to QObject.
  */
QWebServiceMethodAbstract::QWebServiceMethodAbstract(QObject *parent) :
    QObject(parent)
{
}

/*!
    \fn QWebServiceMethodAbstract::QWebServiceMethodAbstract(QSoapMessage *msg, QObject *parent)

    Constructs this object with \a msg (message), and \a parent.
  */
QWebServiceMethodAbstract::QWebServiceMethodAbstract(QSoapMessage *msg, QObject *parent) :
    QObject(parent), message(msg)
{
}

/*!
    \fn QWebServiceMethodAbstract::~QWebServiceMethodAbstract()

    Deletes the message.
  */
QWebServiceMethodAbstract::~QWebServiceMethodAbstract()
{
    delete message;
}

/*!
    \fn QWebServiceMethodAbstract::setHost(QString hostname)

    Sets the host (URL) using \a hostname. Currently empty.
  */
void QWebServiceMethodAbstract::setHost(QString hostname)
{
}

/*!
    \fn QWebServiceMethodAbstract::setHost(QUrl hostUrl)

    Sets the host (URL) using \a hostUrl. Currently empty.
  */
void QWebServiceMethodAbstract::setHost(QUrl hostUrl)
{
}

/*!
    \fn QWebServiceMethodAbstract::getHostUrl()

    Returns host URL in form of a QUrl.
  */
QUrl QWebServiceMethodAbstract::getHostUrl()
{
    return hostUrl;
}

/*!
    \fn QWebServiceMethodAbstract::getHost()

    Returns the hostname (that is, URL in QString).
  */
QString QWebServiceMethodAbstract::getHost()
{
    return host;
}
