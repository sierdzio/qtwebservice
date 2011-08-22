#include <QtTest/QtTest>
#include <qwebservice.h>

/**
  This test case checks both QWebService functionality.
  */

class TestQWebService : public QObject
{
    Q_OBJECT

private slots:
    void initialTest();
};

void TestQWebService::initialTest()
{
    QWebService newService;

    QCOMPARE(newService.isErrorState(), bool(false));
}

QTEST_MAIN(TestQWebService)
#include "tst_qwebservice.moc"

/*
void testCase3_webservice::runWebServiceTest(QString filePathOrUrl)
{

//      Prints out methods with their parameters and return types and names.

    qDebug() << "Running testCase3";
    reader = new QWebServiceReader(filePathOrUrl, this);
    qDebug() << "Getting methods";
    QStringList tempList = reader->methodNames();
    qDebug() << "Listing methods";
    if (!reader->isErrorState())
    {
        int i = 1;
        foreach (QString s, tempList)
        {
            QMap<QString, QVariant> tempParams = reader->parameterNamesTypes(s);
            QString params = "";
            foreach (QString p, tempParams.keys())
            {
                if (params != "")
                    params += ", ";
                params += p + ":" + tempParams.value(p).typeName();
            }

            QMap<QString, QVariant> tempReturns = reader->returnValueNameType(s);
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
*/
