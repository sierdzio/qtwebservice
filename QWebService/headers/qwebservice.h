#ifndef QWEBSERVICE_H
#define QWEBSERVICE_H

#include <QtCore/qurl.h>
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

    QStringList methodNames() const;
    QStringList methodParameters(QString methodName) const;
    QStringList methodReturnValue(QString methodName) const;
    QMap<QString, QVariant> parameterNamesTypes(QString methodName) const;
    QMap<QString, QVariant> returnValueNameType(QString methodName) const;

    void addMethod(QWebServiceMethod *newMethod);
    void addMethod(QString methodName, QWebServiceMethod *newMethod);
    void removeMethod(QString methodName);
    void setHost(QString host);
    void setHost(QUrl hostUrl);
    void setWsdl(QWsdl *newWsdl);
    void resetWsdl(QWsdl *newWsdl = 0);
    QUrl hostUrl() const;
    QString host() const;
    bool isErrorState();
    QString errorInfo() const;
//    QString getWsdl();

signals:
    void errorEncountered(QString errMessage);

protected:
    void init();
    bool enterErrorState(QString errMessage = "");

    bool errorState;
    QString errorMessage;
    QUrl m_hostUrl;
    QWsdl *wsdl;
    // This is general, but should work for custom classes.
    QMap<QString, QWebServiceMethod *> *methods;
};

#endif // QWEBSERVICE_H
