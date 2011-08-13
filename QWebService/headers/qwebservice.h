#ifndef QWEBSERVICEABSTRACT_H
#define QWEBSERVICEABSTRACT_H

#include <QUrl>
#include "QWebService_global.h"
#include "qwebmethod.h"
#include "qwsdl.h"


class QWEBSERVICESHARED_EXPORT QWebService : public QObject
{
    Q_OBJECT

public:
    QWebService(QObject *parent = 0);
    QWebService(QWsdl *wsdl, QObject *parent = 0);
    QWebService(QString host, QObject *parent = 0);
    ~QWebService();

    QStringList getMethodNames();
    QStringList getMethodParameters(QString methodName) const;
    QStringList getMethodReturnValue(QString methodName) const;
    QMap<QString, QVariant> getParameterNamesTypes(QString methodName) const;
    QMap<QString, QVariant> getReturnValueNameType(QString methodName) const;

    void setHost(QString host);
    void setHost(QUrl hostUrl);
    QUrl getHostUrl();
    QString getHost();
    bool isErrorState();
//    QString getWsdl();

protected:
    void init();

    bool errorState;
    QUrl hostUrl;
    QString host;
    QWsdl *wsdl;
    QMap<QString, QWebMethod *> *messages; // This is general, but should work for custom classes.
};

#endif // QWEBSERVICEABSTRACT_H
