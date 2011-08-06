#ifndef QWEBSERVICEREADERABSTRACT_H
#define QWEBSERVICEREADERABSTRACT_H

#include "QWebService_global.h"
#include "qwebserviceabstract.h"

class QWEBSERVICESHARED_EXPORT QWebServiceReaderAbstract : public QWebServiceAbstract
{
    Q_OBJECT

public:
    explicit QWebServiceReaderAbstract(QObject *parent = 0);
    QWebServiceReaderAbstract(QWsdl *wsdl, QObject *parent = 0);
    QWebServiceReaderAbstract(QString hostname, QObject *parent = 0);

signals:

public slots:
    virtual void methodHandler(QVariant param);

protected:

};

#endif // QWEBSERVICEREADERABSTRACT_H
