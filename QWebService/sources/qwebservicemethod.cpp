#include "../headers/qwebservicemethod.h"

/*!
    \class QWebServiceMethod
    \brief Extends QWebMethod with some generic constructors and synchronous message sending.


    Subclass of QWebMethod, contains many generic methods for sending messages. Can be used both synchronously
    (through static sendMessage() method), or asynchronously (indicates, when reply is ready by emitting
    a replyReady() signal).

    More description in the future :)
  */

/*!
    \fn QWebServiceMethod::QWebServiceMethod(QObject *parent)

    Pretty much the default constructor, passes \a parent to QObject.
  */
QWebServiceMethod::QWebServiceMethod(QObject *parent) :
    QWebMethod(parent)
{
}


/*!
    \fn QWebServiceMethod::QWebServiceMethod(QUrl url, QString messageName, QObject *parent, Protocol protocol, HttpMethod method)

    Constructs the message using \a url, \a messageName, \a parent, \a protocol (which defaults to soap12),
    and \a method (which defaults to POST).
    Requires params to be specified later.

    \sa init(), setParameters(), setProtocol(), sendMessage()
  */
QWebServiceMethod::QWebServiceMethod(QUrl url, QString messageName, QObject *parent,
                                     Protocol protocol, HttpMethod method) :
    QWebMethod(parent, protocol, method)
{
    m_hostUrl = url;
    m_methodName = messageName;
    setProtocol(protocol);
    setHttpMethod(method);
}

/*!
    \fn QWebServiceMethod::QWebServiceMethod(QString url, QString messageName, QObject *parent, Protocol protocol, HttpMethod method)

    Constructs the message using \a url, \a messageName, \a parent, \a protocol (which defaults to soap12),
    and \a method (which defaults to POST).
    Requires params to be specified later.

    \sa init(), setParameters(), setProtocol(), sendMessage()
  */
QWebServiceMethod::QWebServiceMethod(QString url, QString messageName, QObject *parent,
                                     Protocol protocol, HttpMethod method) :
    QWebMethod(parent, protocol, method)
{
    m_methodName = messageName;
    setProtocol(protocol);
    setHttpMethod(method);
    m_hostUrl.setUrl(url);
}

/*!
    \fn QWebServiceMethod::QWebServiceMethod(QString url, QString messageName, QString tNamespace, QMap<QString, QVariant> params, QObject *parent, Protocol protocol, HttpMethod method)

    Constructs the message using \a url, \a messageName, \a tNamespace, \a parent, \a protocol (which defaults to soap12),
    and \a method (which defaults to POST).
    This constructor also takes message parameters (\a params).
    Does not require specifying any more information, but you still need to manually send the message
    using sendMessage() (without any arguments, or else - if you want to change ones specified here).

    \sa init(), sendMessage(), setProtocol()
  */
QWebServiceMethod::QWebServiceMethod(QString url, QString messageName, QString tNamespace,
                       QMap<QString, QVariant> params, QObject *parent,
                       Protocol protocol, HttpMethod method) :
    QWebMethod(parent, protocol, method)
{
    m_methodName = messageName;
    m_targetNamespace = tNamespace;
    parameters = params;
    setProtocol(protocol);
    setHttpMethod(method);
    m_hostUrl.setUrl(url);
}


/*!
  \fn bool QWebServiceMethod::sendMessage(QMap<QString, QVariant> params)
  \overload sendMessage()

    Sends the message asynchronously using parameters specified in \a params.

    Returns true on success.
  */
bool QWebServiceMethod::sendMessage(QMap<QString, QVariant> params)
{
    parameters = params;
    sendMessage();
    return true;
}

/*!
  \overload sendMessage()

     STATIC method. Sends the message synchronously, using \a url, \a msgName, \a tNamespace, \a params
     and \a parent.
     Protocol can optionally be specified by \a protocol (default is SOAP 1.2), as well as HTTP \a method
     (default is POST).
     Returns with web service reply.
  */
QByteArray QWebServiceMethod::sendMessage(QObject *parent, QUrl url,
                                 QString msgName, QString tNamespace,
                                 QMap<QString, QVariant> params,
                                 Protocol protocol, HttpMethod method)
{
    /*
       Part of QDoc that does not work:   \fn QVariant QWebServiceMethod::sendMessage(QObject *parent, QUrl url,
       QString _messageName, QMap<QString, QVariant> params,
       QWebServiceMethod::Protocol protocol, QWebServiceMethod::HttpMethod httpMethod)
    */

    QWebServiceMethod qsm(url.toString(), msgName, tNamespace, params, parent, protocol, method);

    qsm.sendMessage();
    // TODO: ADD ERROR HANDLING!
    forever {
        if (qsm.replyReceived) {
            return qsm.reply;
        } else {
            qApp->processEvents();
        }
    }
}
