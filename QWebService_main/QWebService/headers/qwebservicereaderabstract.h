#ifndef QWEBSERVICEREADERABSTRACT_H
#define QWEBSERVICEREADERABSTRACT_H

#include "qwebserviceabstract.h"

class QWebServiceReaderAbstract : public QWebServiceAbstract
{
    Q_OBJECT

public:
    explicit QWebServiceReaderAbstract(QObject *parent = 0);

signals:

public slots:
    virtual void methodHandler(QVariant param);

protected:

};

#endif // QWEBSERVICEREADERABSTRACT_H
