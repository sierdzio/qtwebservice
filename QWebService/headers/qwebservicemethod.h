#ifndef QWEBSERVICEMETHODABSTRACT_H
#define QWEBSERVICEMETHODABSTRACT_H

#include <QUrl>
#include <QObject>
#include "QWebService_global.h"
#include "qwebmethod.h"

class QWEBSERVICESHARED_EXPORT QWebServiceMethod : public QObject
{
    Q_OBJECT

public:
    explicit QWebServiceMethod(QObject *parent = 0);
//    QWebServiceMethodAbstract(QString hostname, QObject *parent = 0);
    QWebServiceMethod(QWebMethod *msg, QObject *parent = 0);
    ~QWebServiceMethod();
    //virtual QWebServiceMethodAbstract(QWsdl wsdl, QObject *parent = 0);
//    bool virtual sendMessage(QList<QVariant> params);
//    bool virtual sendMessage();
    void setHost(QString host);
    void setHost(QUrl hostUrl);
    QUrl getHostUrl();
    QString getHost();
    //QString getWsdl();

signals:
//    void replyRead(QVariant replyVal);

public slots:

protected:
    QUrl hostUrl;
    QString host;
    QWebMethod *message;

};

#endif // QWEBSERVICEMETHODABSTRACT_H
