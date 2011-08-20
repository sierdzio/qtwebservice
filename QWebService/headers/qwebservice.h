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

    QStringList methodNames();
    QStringList methodParameters(QString methodName) const;
    QStringList methodReturnValue(QString methodName) const;
    QMap<QString, QVariant> parameterNamesTypes(QString methodName) const;
    QMap<QString, QVariant> returnValueNameType(QString methodName) const;

    void setHost(QString host);
    void setHost(QUrl hostUrl);
    QUrl hostUrl();
    QString host();
    bool isErrorState();
//    QString getWsdl();

protected:
    void init();

    bool errorState;
    QUrl m_hostUrl;
    QWsdl *wsdl;
    QMap<QString, QWebServiceMethod *> *messages; // This is general, but should work for custom classes.
};

#endif // QWEBSERVICEABSTRACT_H
