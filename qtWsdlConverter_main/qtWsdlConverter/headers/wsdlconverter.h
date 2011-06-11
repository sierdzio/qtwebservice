#ifndef WSDLCONVERTER_H
#define WSDLCONVERTER_H

#include <QObject>
#include "../../../QWebService_main/QWebService/headers/QWebService_global.h"

class WsdlConverter : public QObject
{
    Q_OBJECT

public:
    enum Synchronousness {synchronous, asynchronous};

//    explicit WsdlConverter(QString wsdlFileOrUrl, QObject *parent = 0);
    explicit WsdlConverter(QString wsdlFileOrUrl,
                  QObject *parent = 0,
                  QDir outputDirectory = QDir(),
                  QString baseOutputClassName = "");
    void setFlags(WsdlConverter::Synchronousness synch, QSoapMessage::Protocol);
    void convert();
    ~WsdlConverter();

signals:

public slots:

private:
    void loadMessages();

    QSoapMessage::Protocol soapVersion;
    Synchronousness synchronousness;

    bool errorState;
    QString errorMessage;
    QWsdl *wsdl;
    QMap<QString, QSoapMessage *> *messages;
};

#endif // WSDLCONVERTER_H
