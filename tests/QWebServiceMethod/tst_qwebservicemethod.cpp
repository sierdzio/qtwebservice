/**
    This test checks QWebServiceMethod in operation
    (requires Internet connection or a working local web service)
  */

#include <QtTest/QtTest>
#include <qwebservicemethod.h>

class TestQWebServiceMethod : public QObject
{
    Q_OBJECT

private slots:
    void initialTest();
    void gettersTest();
    void settersTest();
    void asynchronousTest();
    void synchronousTest();

private:
    void defaultGettersTest(QWebServiceMethod *msg);
};

/*
  Performs basic checks of constructor and basic methods.
  */
void TestQWebServiceMethod::initialTest()
{
    QWebServiceMethod *message = new QWebServiceMethod(0, QWebMethod::soap12, QWebMethod::POST);
    QCOMPARE(message->isErrorState(), bool(false));

    delete message;
}

/*
  Performs basic checks of getters.
  */
void TestQWebServiceMethod::gettersTest()
{
    QWebServiceMethod *message = new QWebServiceMethod(0, QWebMethod::soap12, QWebMethod::POST);
    defaultGettersTest(message);
    delete message;
}

/*
  Performs basic checks of setters.
  */
void TestQWebServiceMethod::settersTest()
{
    QWebServiceMethod *message = new QWebServiceMethod(0, QWebMethod::soap12, QWebMethod::POST);
    defaultGettersTest(message);

    QUrl tempUrl("http://www.currencyserver.de/webservice/currencyserverwebservice.asmx");
    message->setHost(tempUrl);
    QCOMPARE(message->hostUrl(), tempUrl);

    QString tempName = "getProviderList";
    message->setMessageName(tempName);
    QCOMPARE(message->methodName(), tempName);

    QString tempTargetNmspc = "http://www.daenet.de/webservices/CurrencyServer";
    message->setTargetNamespace(tempTargetNmspc);
    QCOMPARE(message->targetNamespace(), tempTargetNmspc);

    message->setProtocol(QWebMethod::json);
    QCOMPARE(message->protocol(), QWebMethod::json);
    QCOMPARE(message->protocolString(), QString("json"));
    QCOMPARE(message->protocolString(true), QString("json"));

    message->setHttpMethod(QWebMethod::DELETE);
    QCOMPARE(message->httpMethod(), QWebMethod::DELETE);
    QCOMPARE(message->httpMethodString(), QString("DELETE"));

    message->setHttpMethod("get");
    QCOMPARE(message->httpMethod(), QWebMethod::GET);
    QCOMPARE(message->httpMethodString(), QString("GET"));

    message->setHttpMethod("pUT");
    QCOMPARE(message->httpMethod(), QWebMethod::PUT);
    QCOMPARE(message->httpMethodString(), QString("PUT"));

    message->setHttpMethod("POST");
    QCOMPARE(message->httpMethod(), QWebMethod::POST);
    QCOMPARE(message->httpMethodString(), QString("POST"));

    QMap<QString, QVariant> tmpP;
    tmpP.insert("symbol", QVariant("NOK"));
    message->setParameters(tmpP);
    QCOMPARE(message->parameterNames().size(), int(1));
    QCOMPARE(message->parameterNames().first(), QString("symbol"));
    QCOMPARE(message->parameterNamesTypes().value("symbol"), QVariant("NOK"));

    message->setReturnValue(tmpP);
    QCOMPARE(message->returnValueName().size(), int(1));
    QCOMPARE(message->returnValueName().first(), QString("symbol"));
    QCOMPARE(message->returnValueNameType().value("symbol"), QVariant("NOK"));

    delete message;
}

/*
  This test requires Internet connection!
  */
void TestQWebServiceMethod::asynchronousTest()
{
    // Message parameters are specified here.
    QMap<QString, QVariant> tmpP;
    QWebServiceMethod *message = new
            QWebServiceMethod("http://www.currencyserver.de/webservice/currencyserverwebservice.asmx",
                              "getProviderList",
                              this, QWebMethod::soap12, QWebMethod::POST);

    message->setTargetNamespace("http://www.daenet.de/webservices/CurrencyServer");
    message->sendMessage(tmpP);

    QCOMPARE(message->isErrorState(), bool(false));

    bool result = false;
    for (int i = 0; (i < 50) && (!message->isReplyReady()); i++)
        QTest::qWait(250);

    if (message->isReplyReady()) {
        result = true;
    }

    QCOMPARE(result, bool(true));
    delete message;
}

/*
  This test requires Internet connection!
  */
void TestQWebServiceMethod::synchronousTest()
{
    // Message parameters are specified here.
    QMap<QString, QVariant> tmpP;
    bool result = false;
    QString reply = "pass";
    reply = QWebServiceMethod::sendMessage(this,
             QUrl("http://www.currencyserver.de/webservice/currencyserverwebservice.asmx"),
             "getProviderList", "http://www.daenet.de/webservices/CurrencyServer",
             tmpP, QWebMethod::soap12);

    if ((reply != "") && (reply != "pass"))
    {
        result = true;
    }

    QCOMPARE(result, bool(true));
}


void TestQWebServiceMethod::defaultGettersTest(QWebServiceMethod *message)
{
    QCOMPARE(message->isErrorState(), bool(false));
    QCOMPARE(message->isReplyReady(), bool(false));
    QCOMPARE(message->errorInfo(), QString(""));
    QCOMPARE(message->httpMethodString(), QString("POST"));
    QCOMPARE(message->httpMethod(), QWebMethod::POST);
    QCOMPARE(message->protocolString(false), QString("soap12"));
    QCOMPARE(message->protocolString(true), QString("soap12"));
    QCOMPARE(message->protocol(), QWebMethod::soap12);
    QCOMPARE(message->hostUrl(), QUrl(""));
    QCOMPARE(message->host(), QString(""));
    QCOMPARE(message->targetNamespace(), QString(""));
    QCOMPARE(message->methodName(), QString(""));
    QCOMPARE(message->returnValueNameType().size(), int(0));
    QCOMPARE(message->returnValueName().size(), int(0));
    QCOMPARE(message->parameterNamesTypes().size(), int(0));
    QCOMPARE(message->parameterNames().size(), int(0));
}

QTEST_MAIN(TestQWebServiceMethod)
#include "tst_qwebservicemethod.moc"
