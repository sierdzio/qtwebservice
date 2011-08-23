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
//    void synchronousTest();
};

void TestQWebServiceMethod::asynchronousTest()
{
    // Message parameters are specified here.
    QMap<QString, QVariant> tmpP;
    tmpP.insert("symbol", QVariant("NOK"));

    QWebServiceMethod *message = new QWebServiceMethod("http://www.webservicex.net/stockquote.asmx", "GetQuote",
                                                        tmpP, this, QWebMethod::soap12, QWebMethod::POST);

//    connect(message, SIGNAL(replyReady(QVariant)), this, SLOT(messageResponse(QVariant)));

    message->setTargetNamespace("http://www.webserviceX.NET/");
//    message->sendMessage();

    QCOMPARE(message->isErrorState(), bool(false));

//    QTest::qWait(10000);
//    bool result = false;
//    if (message->isReplyReady()) {
//            result = true;
//        }

//    QCOMPARE(result, bool(true));

    delete message;
}
/*
void TestQWebServiceMethod::synchronousTest()
{
    // Message parameters are specified here.
    QMap<QString, QVariant> tmpP;
    tmpP.insert("symbol", QVariant("NOK"));

    QString result = "";
    result = QWebServiceMethod::sendMessage(this, QUrl("http://www.webservicex.net/stockquote.asmx"),
                                            "GetQuote", tmpP, QWebMethod::soap12).toString();

    QEXPECT_FAIL("", "Expected non-empty string", Continue);
    QCOMPARE(result, QString(""));
}
*/
QTEST_MAIN(TestQWebServiceMethod)
#include "tst_qwebservicemethod.moc"
