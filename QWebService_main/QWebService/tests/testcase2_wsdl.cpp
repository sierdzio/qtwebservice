#include "testcase2_wsdl.h"

testCase2_wsdl::testCase2_wsdl(QWidget *parent) :
    QPushButton(parent)
{
    runWsdlTest("http://www.webservicex.net/LondonGoldFix.asmx?WSDL");
}

testCase2_wsdl::~testCase2_wsdl()
{
    delete wsdl;
}

void testCase2_wsdl::runWsdlTest(QString filePathOrUrl)
{
    qDebug() << "Running testCase2";
    wsdl = new QWsdl(filePathOrUrl, this);
    QStringList tempList = wsdl->getMethodNames();

    if (!wsdl->isErrorState())
    {
        int i = 1;
        foreach (QString s, tempList)
        {
            qDebug() << i << ": " << s;
            i++;
        }
    }
    else
    {
        qDebug() << "QWsdl entered the error state and could not continue.";
    }
}
