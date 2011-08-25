#ifndef QWEBSERVICEREADER_H
#define QWEBSERVICEREADER_H

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>
#include "QWebService_global.h"
#include "qwsdl.h"
#include "qwebservice.h"

class QWEBSERVICESHARED_EXPORT QWebServiceReader : public QWebService
{
    Q_OBJECT

public:
    explicit QWebServiceReader(QObject *parent = 0);
    QWebServiceReader(QWsdl *wsdl, QObject *parent = 0);
    QWebServiceReader(QString host, QObject *parent = 0);

public slots:
    virtual void methodHandler(QVariant param);
};

#endif // QWEBSERVICEREADER_H
