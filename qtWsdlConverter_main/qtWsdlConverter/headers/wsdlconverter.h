#ifndef WSDLCONVERTER_H
#define WSDLCONVERTER_H

#include <QObject>
#include "../../../QWebService_main/QWebService/headers/QWebService_global.h"
#include "standardpath.h"

class WsdlConverter : public QObject
{
    Q_OBJECT

public:
    enum Synchronousness {synchronous, asynchronous};
    enum Structure {standardStructure, noMessagesStructure};

//    explicit WsdlConverter(QString wsdlFileOrUrl, QObject *parent = 0);
    explicit WsdlConverter(QString wsdlFileOrUrl,
                  QObject *parent = 0,
                  QDir outputDirectory = QDir(),
                  QString baseOutputClassName = "");
    ~WsdlConverter();

    void setFlags(WsdlConverter::Synchronousness synchronousness, QSoapMessage::Protocol protocol);
    bool isErrorState();
    void convert();
    QString getWebServiceName();

signals:
    void errorEncountered(QString errMessage);

public slots:

private:
//    bool createDirs();
    void enterErrorState(QString errMessage = "");
//    void setFlagsOnMessages();
//    void loadMessages();

    QSoapMessage::Protocol protocol;
    Synchronousness synchronousness;
    Structure structure;

    bool errorState;
    QString errorMessage;
    QWsdl *wsdl;
//    QMap<QString, QSoapMessage *> *messages;
};

#endif // WSDLCONVERTER_H
