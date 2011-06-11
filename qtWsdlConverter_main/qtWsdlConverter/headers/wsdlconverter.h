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
    ~WsdlConverter();

    void setFlags(WsdlConverter::Synchronousness synchronousness, QSoapMessage::Protocol protocol);
    void convert();
    bool isErrorState();

signals:
    void errorEncountered(QString errMessage);

public slots:

private:
    void loadMessages();
    void setFlagsOnMessages();
    void enterErrorState(QString errMessage = "");

    QSoapMessage::Protocol protocol;
    Synchronousness synchronousness;

    bool errorState;
    QString errorMessage;
    QWsdl *wsdl;
    QMap<QString, QSoapMessage *> *messages;
};

#endif // WSDLCONVERTER_H
