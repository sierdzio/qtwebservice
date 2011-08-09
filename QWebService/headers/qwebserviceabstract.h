#ifndef QWEBSERVICEABSTRACT_H
#define QWEBSERVICEABSTRACT_H

#include <QUrl>
#include "QWebService_global.h"
#include "qsoapmessage.h"
#include "qwsdl.h"


class QWEBSERVICESHARED_EXPORT QWebServiceAbstract : public QObject //QWEBSERVICESHARED_EXPORT
{
    Q_OBJECT

public:
    QWebServiceAbstract(QObject *parent = 0);
    QWebServiceAbstract(QWsdl *wsdl, QObject *parent = 0);
    QWebServiceAbstract(QString host, QObject *parent = 0);
    ~QWebServiceAbstract();

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
    QMap<QString, QSoapMessage *> *messages; // This is general, but should work for custom classes.
};

#endif // QWEBSERVICEABSTRACT_H
