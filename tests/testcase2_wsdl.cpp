#include "testcase2_wsdl.h"

testCase2_wsdl::testCase2_wsdl(QWidget *parent) :
    QPushButton(parent)
{
//    runWsdlTest("http://www.webservicex.net/LondonGoldFix.asmx?WSDL");
//    runWsdlTest("http://www.webservicex.net/stockquote.asmx?WSDL");
    runWsdlTest("/home/sierdzio/Dropbox/Code/QWebService/QWebService_main/QWebService/examples/band_ws.asmx");
}

testCase2_wsdl::~testCase2_wsdl()
{
    delete wsdl;
}

void testCase2_wsdl::runWsdlTest(QString filePathOrUrl)
{
    qDebug() << "Running testCase2";
    wsdl = new QWsdl(filePathOrUrl, this);
    qDebug() << "Getting methods";
    QStringList tempList = wsdl->getMethodNames();
    qDebug() << "Listing methods";
    if (!wsdl->isErrorState())
    {
        int i = 1;
        foreach (QString s, tempList)
        {
            qDebug() << i << ": " << s;
            i++;
        }
        qDebug() << "Done";
    }
    else
    {
        qDebug() << "QWsdl entered the error state and could not continue.";
    }
}
