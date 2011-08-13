#include "../headers/qwebservicemethod.h"

/*!
    \class QWebServiceMethod
    \brief Unused class, was meant to be a high-level wrapper for QWebMethod.

    This class will probably be dropped, or at least seriously refactored in the future.
  */

/*!
    \fn QWebServiceMethod::QWebServiceMethod(QObject *parent)

    Pretty much the default constructor, passes \a parent to QObject.
  */
QWebServiceMethod::QWebServiceMethod(QObject *parent) :
    QObject(parent)
{
}

/*!
    \fn QWebServiceMethod::QWebServiceMethod(QWebMethod *msg, QObject *parent)

    Constructs this object with \a msg (message), and \a parent.
  */
QWebServiceMethod::QWebServiceMethod(QWebMethod *msg, QObject *parent) :
    QObject(parent), message(msg)
{
}

/*!
    \fn QWebServiceMethod::~QWebServiceMethod()

    Deletes the message.
  */
QWebServiceMethod::~QWebServiceMethod()
{
    delete message;
}

/*!
    \fn QWebServiceMethod::setHost(QString hostname)

    Sets the host (URL) using \a hostname. Currently empty.
  */
void QWebServiceMethod::setHost(QString hostname)
{
}

/*!
    \fn QWebServiceMethod::setHost(QUrl hostUrl)

    Sets the host (URL) using \a hostUrl. Currently empty.
  */
void QWebServiceMethod::setHost(QUrl hostUrl)
{
}

/*!
    \fn QWebServiceMethod::getHostUrl()

    Returns host URL in form of a QUrl.
  */
QUrl QWebServiceMethod::getHostUrl()
{
    return hostUrl;
}

/*!
    \fn QWebServiceMethod::getHost()

    Returns the hostname (that is, URL in QString).
  */
QString QWebServiceMethod::getHost()
{
    return host;
}
