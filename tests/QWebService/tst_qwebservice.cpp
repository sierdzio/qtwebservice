#include <QtTest/QtTest>
#include <qwebservicereader.h>

/**
  This test case checks both QWebService (and QWebServiceReader) functionality.
  */

class TestQWebService : public QObject
{
    Q_OBJECT

private slots:
    void initialTest();
    void gettersTest();
    void settersTest();
    void methodManagementTest();
};

/*
  Performs basic checks of constructor and basic methods.
  */
void TestQWebService::initialTest()
{
    QWebServiceReader *reader1;
    reader1 = new QWebServiceReader(this);
    QCOMPARE(reader1->isErrorState(), bool(false));

    QWebServiceReader *reader2;
    reader2 = new QWebServiceReader("../../examples/wsdl/band_ws.asmx", this);
    QCOMPARE(reader2->isErrorState(), bool(false));

    QWebServiceReader *reader3;
    QWsdl *wsdl = new QWsdl("../../examples/wsdl/band_ws.asmx", this);
    reader3 = new QWebServiceReader(wsdl, this);
    QCOMPARE(reader3->isErrorState(), bool(false));

    delete reader1;
    delete reader2;
    delete reader3;
}

/*
  Performs basic checks of getters.
  */
void TestQWebService::gettersTest()
{
    QWebServiceReader reader("../../examples/wsdl/band_ws.asmx", this);

    QCOMPARE(reader.host(), QString(""));
    QCOMPARE(reader.hostUrl(), QUrl(""));
    QCOMPARE(reader.isErrorState(), bool(false));
    QCOMPARE(reader.errorInfo(), QString(""));
    QCOMPARE(reader.methodNames().size(), int(13));

    QStringList tempList = reader.methodNames();

    foreach (QString s, tempList) {
        QMap<QString, QVariant> tempParams = reader.parameterNamesTypes(s);
        QMap<QString, QVariant> tempReturns = reader.returnValueNameType(s);

        if (s == "getBandName") {
            QCOMPARE(tempParams.size(), int(1));
            QCOMPARE(tempReturns.size(), int(1));
        }
        else if (s == "getBandDescription") {
            QCOMPARE(tempParams.size(), int(1));
            QCOMPARE(tempReturns.size(), int(1));
        }
        else if (s == "getBandsList") {
            QCOMPARE(tempParams.size(), int(0));
            QCOMPARE(tempReturns.size(), int(1));
        }
        else if (s == "getBandsListForGenreAndDate") {
            QCOMPARE(tempParams.size(), int(2));
            QCOMPARE(tempReturns.size(), int(1));
        }
        else if (s == "getBandsListForGenre") {
            QCOMPARE(tempParams.size(), int(1));
            QCOMPARE(tempReturns.size(), int(1));
        }
        else if (s == "getGenreList") {
            QCOMPARE(tempParams.size(), int(0));
            QCOMPARE(tempReturns.size(), int(1));
        }
        else if (s == "getBandPricePerShow") {
            QCOMPARE(tempParams.size(), int(1));
            QCOMPARE(tempReturns.size(), int(1));
        }
        else if (s == "getBandsForADate") {
            QCOMPARE(tempParams.size(), int(1));
            QCOMPARE(tempReturns.size(), int(1));
        }
        else if (s == "getNextEmptySlot") {
            QCOMPARE(tempParams.size(), int(2));
            QCOMPARE(tempReturns.size(), int(1));
        }
        else if (s == "bookABand") {
            QCOMPARE(tempParams.size(), int(11));
            QCOMPARE(tempReturns.size(), int(1));
        }
        else if (s == "cancelBookingById") {
            QCOMPARE(tempParams.size(), int(1));
            QCOMPARE(tempReturns.size(), int(1));
        }
        else if (s == "cancelBookingById") {
            QCOMPARE(tempParams.size(), int(3));
            QCOMPARE(tempReturns.size(), int(1));
        }
        else if (s == "getBandSchedule") {
            QCOMPARE(tempParams.size(), int(1));
            QCOMPARE(tempReturns.size(), int(1));
        }
    }
}

/*
  Performs basic checks of setters.
  */
void TestQWebService::settersTest()
{
    QWebServiceReader *reader;
    reader = new QWebServiceReader("../../examples/wsdl/band_ws.asmx", this);
    QCOMPARE(reader->isErrorState(), bool(false));

    reader->setHost("../../examples/wsdl/LondonGoldFix.asmx.xml");
    QCOMPARE(reader->isErrorState(), bool(false));
    QCOMPARE(reader->host(), QString("../../examples/wsdl/LondonGoldFix.asmx.xml"));
    QCOMPARE(reader->hostUrl(), QUrl("../../examples/wsdl/LondonGoldFix.asmx.xml"));
    QCOMPARE(reader->errorInfo(), QString(""));
    QCOMPARE(reader->methodNames().size(), int(1));

    reader->setHost("../../examples/wsdl/stockquote.asmx");
    QCOMPARE(reader->isErrorState(), bool(false));
    QCOMPARE(reader->host(), QString("../../examples/wsdl/stockquote.asmx"));
    QCOMPARE(reader->hostUrl(), QUrl("../../examples/wsdl/stockquote.asmx"));
    QCOMPARE(reader->errorInfo(), QString(""));
    qDebug() << reader->methodNames();
    QCOMPARE(reader->methodNames().size(), int(1));;

    delete reader;
}

/*
  Tests adding and removing web methods - boths by QWsdl and custom ones.
  */
void TestQWebService::methodManagementTest()
{
    QWebServiceReader *reader;
    reader = new QWebServiceReader(this);
    QCOMPARE(reader->isErrorState(), bool(false));

    reader->setWsdl(new QWsdl("../../examples/wsdl/band_ws.asmx", reader));
    QCOMPARE(reader->isErrorState(), bool(false));
    QCOMPARE(reader->methodNames().size(), int(13));
    reader->resetWsdl();
    QCOMPARE(reader->isErrorState(), bool(false));
    QCOMPARE(reader->methodNames().size(), int(0));

    reader->addMethod("testMethod1", new QWebServiceMethod(reader));
    QCOMPARE(reader->isErrorState(), bool(false));
    QCOMPARE(reader->methodNames().size(), int(1));
    reader->removeMethod("testMethod1");
    QCOMPARE(reader->isErrorState(), bool(false));
    QCOMPARE(reader->methodNames().size(), int(0));

    delete reader;
}

QTEST_MAIN(TestQWebService)
#include "tst_qwebservice.moc"
