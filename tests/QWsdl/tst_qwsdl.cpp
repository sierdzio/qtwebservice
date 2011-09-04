#include <QtTest/QtTest>
#include <qwsdl.h>

/*
  This test tests QWsdl operation.

  It does not require Internet connection - you can check a physical WSDL file if you want to,
  or specify address to where a remote WSDL resides.
  */

class TestQWsdl : public QObject
{
    Q_OBJECT

private slots:
    void initialTest();
    void gettersTest();
    void settersTest();
};

/*
  Performs basic checks of constructor and basic methods.
  */
void TestQWsdl::initialTest()
{
    QWsdl *wsdl1 = new QWsdl(this);
    QCOMPARE(wsdl1->isErrorState(), bool(false));
    delete wsdl1;

    QWsdl wsdl2("../../examples/wsdl/band_ws.asmx", this);
    QCOMPARE(wsdl2.isErrorState(), bool(false));

    QStringList tempList = wsdl2.methodNames();
    QCOMPARE(tempList.size(), int(13));
}

/*
  Performs basic checks of getters.
  */
void TestQWsdl::gettersTest()
{
    QWsdl wsdl("../../examples/wsdl/band_ws.asmx", this);

    QCOMPARE(wsdl.host(), QString("/band_ws.asmx"));
    QCOMPARE(wsdl.hostUrl(), QUrl("http://localhost:1304/band_ws.asmx"));
    QCOMPARE(wsdl.wsdlFile(), QString("../../examples/wsdl/band_ws.asmx"));
    QCOMPARE(wsdl.webServiceName(), QString("band_ws"));
    QCOMPARE(wsdl.targetNamespace(), QString("http://tempuri.org/"));
    QCOMPARE(wsdl.isErrorState(), bool(false));
    QCOMPARE(wsdl.errorInfo(), QString(""));
    QCOMPARE(wsdl.methodNames().size(), int(13));

    QStringList tempList = wsdl.methodNames();
    QMap<QString, QWebServiceMethod *> *methods = wsdl.methods();

    foreach (QString s, tempList) {
        QMap<QString, QVariant> tempParams = methods->value(s)->parameterNamesTypes();
        QMap<QString, QVariant> tempReturns = methods->value(s)->returnValueNameType();

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
void TestQWsdl::settersTest()
{
    QWsdl *wsdl;
    wsdl = new QWsdl("../../examples/wsdl/band_ws.asmx", this);
    QCOMPARE(wsdl->isErrorState(), bool(false));

    wsdl->resetWsdl("../../examples/wsdl/LondonGoldFix.asmx.xml");
    QCOMPARE(wsdl->isErrorState(), bool(false));
    QCOMPARE(wsdl->host(), QString("/LondonGoldFix.asmx"));
    QCOMPARE(wsdl->hostUrl(), QUrl("http://www.webservicex.net/LondonGoldFix.asmx"));
    QCOMPARE(wsdl->wsdlFile(), QString("../../examples/wsdl/LondonGoldFix.asmx.xml"));
    QCOMPARE(wsdl->webServiceName(), QString("LondonGoldAndSilverFix"));
    QCOMPARE(wsdl->targetNamespace(), QString("http://www.webservicex.net"));
    QCOMPARE(wsdl->errorInfo(), QString(""));
    QCOMPARE(wsdl->methodNames().size(), int(1));

    wsdl->setWsdlFile("../../examples/wsdl/stockquote.asmx");
    QCOMPARE(wsdl->isErrorState(), bool(false));
    QCOMPARE(wsdl->host(), QString("/stockquote.asmx"));
    QCOMPARE(wsdl->hostUrl(), QUrl("http://www.webservicex.net/stockquote.asmx"));
    QCOMPARE(wsdl->wsdlFile(), QString("../../examples/wsdl/stockquote.asmx"));
    QCOMPARE(wsdl->webServiceName(), QString("StockQuote"));
    QCOMPARE(wsdl->targetNamespace(), QString("http://www.webserviceX.NET/"));
    QCOMPARE(wsdl->errorInfo(), QString(""));
    QCOMPARE(wsdl->methodNames().size(), int(1));;

    delete wsdl;
}

QTEST_MAIN(TestQWsdl)
#include "tst_qwsdl.moc"

