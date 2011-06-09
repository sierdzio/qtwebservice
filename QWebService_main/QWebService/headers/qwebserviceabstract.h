#ifndef QWEBSERVICEABSTRACT_H
#define QWEBSERVICEABSTRACT_H

#include <QUrl>
#include "QWebService_global.h"

class QWEBSERVICESHARED_EXPORT QWebServiceAbstract : public QObject
{
    Q_OBJECT

public:
    QWebServiceAbstract(QObject *parent = 0);
    QWebServiceAbstract(QWsdl wsdl, QObject *parent = 0);
    QWebServiceAbstract(QString hostname, QObject *parent = 0);
    QStringList getMethodNames();
    void setHost(QString hostname);
    void setHost(QUrl hostUrl);
    QUrl getHostUrl();
    QString getHost();
//    QString getWsdl();

protected:
    QUrl hostUrl;
    QString hostname;
    QWsdl *wsdl;
};

#endif // QWEBSERVICEABSTRACT_H
