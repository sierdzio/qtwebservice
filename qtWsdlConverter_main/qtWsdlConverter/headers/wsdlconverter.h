#ifndef WSDLCONVERTER_H
#define WSDLCONVERTER_H

#include <QObject>
#include "../../../QWebService_main/QWebService/headers/QWebService_global.h"

class WsdlConverter : public QObject
{
    Q_OBJECT

public:
    explicit WsdlConverter(QString wsdlFileOrUrl, QObject *parent = 0);
    ~WsdlConverter();

signals:

public slots:

private:
    bool errorState;
    QString errorMessage;
    QWsdl *wsdl;
};

#endif // WSDLCONVERTER_H
