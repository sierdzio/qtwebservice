#ifndef STANDARDPATH_H
#define STANDARDPATH_H

#include <QObject>
#include "../../../QWebService_main/QWebService/headers/QWebService_global.h"
#include "flags.h"

class StandardPath : public QObject
{
    Q_OBJECT
public:
    explicit StandardPath(QObject *parent = 0);
    static bool create(QWsdl *wsdl, QDir workingDir, Flags flgs, QObject *parent = 0);

signals:

public slots:

private:
    void prepare();
    bool createMessages();
    bool createMessageHeader(QSoapMessage *msg);
    bool createMessageSource(QSoapMessage *msg);
    bool createService();
    bool createServiceHeader();
    bool createServiceSource();
    bool createQMakeProject();

    QMap<QString, QSoapMessage *> *messages;
    QDir workingDir;
    QWsdl *wsdl;
    Flags flags;

};

#endif // STANDARDPATH_H
