#ifndef TESTCASE3_WEBSERVICE_H
#define TESTCASE3_WEBSERVICE_H

#include <QPushButton>
//#include "../headers/qwsdl.h"
//#include "../headers/qwebserviceabstract.h"
#include "../QWebService/headers/qwebservicereaderabstract.h"
//#include "../headers/qwebservicemethodabstract.h"

class testCase3_webservice : public QPushButton
{
    Q_OBJECT

public:
    explicit testCase3_webservice(QPushButton *parent = 0);
    ~testCase3_webservice();

signals:

public slots:

private:
    void runWebServiceTest(QString filePathOrUrl);

    QWebServiceReaderAbstract *reader;
//    QWsdl *wsdl;
};

#endif // TESTCASE3_WEBSERVICE_H
