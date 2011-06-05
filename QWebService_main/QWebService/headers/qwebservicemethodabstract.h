#ifndef QWEBSERVICEMETHODABSTRACT_H
#define QWEBSERVICEMETHODABSTRACT_H

#include <QUrl>
#include <QObject>
#include "qsoapmessage.h"

class QWebServiceMethodAbstract : public QObject
{
    Q_OBJECT

public:
    explicit QWebServiceMethodAbstract(QObject *parent = 0);
    QWebServiceMethodAbstract(QString hostname, QObject *parent = 0);
    //virtual QWebServiceMethodAbstract(QWsdl wsdl, QObject *parent = 0);
    bool virtual sendMessage(QList<QVariant> params);
    bool virtual sendMessage();
    void setHost(QString hostname);
    void setHost(QUrl hostUrl);
    QUrl getHostUrl();
    QString getHost();
    //QString getWsdl();

signals:
    void replyRead(QVariant replyVal);

public slots:

protected:
    QUrl hostUrl;
    QString host;
    QSoapMessage message;

};

#endif // QWEBSERVICEMETHODABSTRACT_H
