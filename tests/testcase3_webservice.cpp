#include "testcase3_webservice.h"

/**
  This test case combines previosu two - it checks both QWebMethod and QWsdl functionality,
  in a broad spectrum of uses.
  */
testCase3_webservice::testCase3_webservice(QPushButton *parent) :
    QPushButton(parent)
{
    /**
      Check based on a local WSDL file, placed in 'examples' folder.
      You can specify your own, or even a web service URL.
      */
    runWebServiceTest("../examples/band_ws.asmx");
}

testCase3_webservice::~testCase3_webservice()
{
    delete reader;
    //delete wsdl;
}

void testCase3_webservice::runWebServiceTest(QString filePathOrUrl)
{
    /**
      Prints out methods with their parameters and return types and names.
      */
    qDebug() << "Running testCase3";
    reader = new QWebServiceReader(filePathOrUrl, this);
    qDebug() << "Getting methods";
    QStringList tempList = reader->getMethodNames();
    qDebug() << "Listing methods";
    if (!reader->isErrorState())
    {
        int i = 1;
        foreach (QString s, tempList)
        {
            QMap<QString, QVariant> tempParams = reader->getParameterNamesTypes(s);
            QString params = "";
            foreach (QString p, tempParams.keys())
            {
                if (params != "")
                    params += ", ";
                params += p + ":" + tempParams.value(p).typeName();
            }

            QMap<QString, QVariant> tempReturns = reader->getReturnValueNameType(s);
            QString returns = "";
            foreach (QString r, tempReturns.keys())
            {
                if (returns != "")
                    returns += ", ";
                returns += r + ":" + tempReturns.value(r).typeName();
            }

            qDebug() << i << ": " << s;
            qDebug() << "\tParams: " << params;
            qDebug() << "\tReturn: " << returns;
            i++;
        }
        qDebug() << "Done";
    }
    else
    {
        qDebug() << "QWebServiceAbstract entered the error state and could not continue.";
    }
}
