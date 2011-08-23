#ifndef QWEBSERVICEREADER_H
#define QWEBSERVICEREADER_H

#include "QWebService_global.h"
#include "qwebservice.h"

class QWEBSERVICESHARED_EXPORT QWebServiceReader : public QWebService
{
    Q_OBJECT

public:
    explicit QWebServiceReader(QObject *parent = 0);
    QWebServiceReader(QWsdl *wsdl, QObject *parent = 0);
    QWebServiceReader(QString host, QObject *parent = 0);

signals:

public slots:
    virtual void methodHandler(QVariant param);

protected:

};

#endif // QWEBSERVICEREADER_H
