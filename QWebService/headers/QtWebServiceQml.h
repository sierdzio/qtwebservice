#ifndef QTWEBSERVICEQML_H
#define QTWEBSERVICEQML_H

//#include "QWebService"
#include "qwebmethod.h"
#include "qwebservicemethod.h"
#include "qwsdl.h"
#include "qwebservice.h"

void registerQmlTypes() {
    qmlRegisterType<QWebMethod>("QtWebService", 0, 6, "WebMethod");
    qmlRegisterType<QWebServiceMethod>("QtWebService", 0, 6, "WebServiceMethod");
    qmlRegisterType<QWsdl>("QtWebService", 0, 6, "Wsdl");
//    qmlRegisterType<QWebService>("QtWebService", 0, 6, "WebService");
}

#endif // QTWEBSERVICEQML_H
