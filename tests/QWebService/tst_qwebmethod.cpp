/**
  This test checks QWebMethod in operation (requires Internet connection or a working local web service)
  */

#include <QtCore>
#include <QtTest/QtTest>
#include <qwebmethod.h>

class TestQWebMethod : public QObject
{
    Q_OBJECT

private slots:
    void cleanRun();
};

void TestQWebMethod::cleanRun()
{
    QMap<QString, QVariant> tmpP;
    tmpP.insert("symbol", QVariant("NOK"));

//    QWebMethod message("http://www.webservicex.net/stockquote.asmx", "GetQuote", tmpP, this);
    QWebMethod message;
    message.setTargetNamespace("http://www.webserviceX.NET/");

    QCOMPARE(message.isErrorState(), bool(false));
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
