/**
  This test checks QWebMethod in operation (requires Internet connection or a working local web service)
  */

#include <QtTest/QtTest>
//#include <QtCore>
//#include <qwebmethod.h>
#include <qwebservicemethod.h>

class TestQWebServiceMethod : public QObject
{
    Q_OBJECT

private slots:
    void asynchronousTest();
    void synchronousTest();
};

void TestQWebServiceMethod::asynchronousTest()
{
    // Message parameters are specified here.
    QMap<QString, QVariant> tmpP;
//    tmpP.insert("symbol", QVariant("NOK"));

    QWebServiceMethod *message = new
            QWebServiceMethod("http://www.currencyserver.de/webservice/currencyserverwebservice.asmx",
                              "getProviderList",
                              this, QWebMethod::soap12, QWebMethod::POST);

//    connect(message, SIGNAL(replyReady(QVariant)), this, SLOT(messageResponse(QVariant)));

    message->setTargetNamespace("http://www.daenet.de/webservices/CurrencyServer");
    message->sendMessage(tmpP);

    QCOMPARE(message->isErrorState(), bool(false));

    bool result = false;
    for (int i = 0; (i < 50) && (!message->isReplyReady()); i++)
        QTest::qWait(250);

    if (message->isReplyReady()) {
        result = true;
//        qDebug() << message->replyRead().toString();
    }

    QCOMPARE(result, bool(true));
    delete message;
}

void TestQWebServiceMethod::synchronousTest()
{
    // Message parameters are specified here.
    QMap<QString, QVariant> tmpP;
//    tmpP.insert("symbol", QVariant("NOK"));

    bool result = false;
    QString reply = "pass";
    reply = QWebServiceMethod::sendMessage(this,
                                           QUrl("http://www.currencyserver.de/webservice/currencyserverwebservice.asmx"),
                                           "getProviderList", "http://www.daenet.de/webservices/CurrencyServer",
                                           tmpP, QWebMethod::soap12).toString();

    if ((reply != "") && (reply != "pass"))
    {
        qDebug() << reply;
        result = true;
    }

    QCOMPARE(result, bool(true));
}

QTEST_MAIN(TestQWebServiceMethod)
#include "tst_qwebservicemethod.moc"
