/**
  This test checks QWebMethod in operation (requires Internet connection or a working local web service)
  */

#include <QtTest/QtTest>
//#include <QtCore>
#include <qwebmethod.h>

class TestQWebMethod : public QObject
{
    Q_OBJECT

private slots:
    void initialTest();
};

void TestQWebMethod::initialTest()
{
    QWebMethod *message = new QWebMethod(0, QWebMethod::soap12, QWebMethod::POST);

    QCOMPARE(message->isErrorState(), bool(false));

    delete message;
}

QTEST_MAIN(TestQWebMethod)
#include "tst_qwebmethod.moc"

/*
void testCase1::messageResponse(QVariant msg)
{
    QString rply = msg.toString();
    qDebug() << rply;
}

void testCase1::soapMessageTest()
{
    // Message parameters are specified here.
    QMap<QString, QVariant> tmpP;
    tmpP.insert("symbol", QVariant("NOK"));

    // This is the reply holder.
    QMap<QString, QVariant> tmpR;
    tmpR.insert("GetQuoteResponse", QVariant("s:string"));

    message = new QWebServiceMethod("http://www.webservicex.net/stockquote.asmx", "GetQuote", tmpP, this);

    connect(message, SIGNAL(replyReady(QVariant)), this, SLOT(messageResponse(QVariant)));

    message->setTargetNamespace("http://www.webserviceX.NET/");
    message->sendMessage();
}
*/
