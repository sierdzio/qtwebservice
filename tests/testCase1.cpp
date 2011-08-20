#include "testCase1.h"

/**
  This test checks QWebMethod in operation (requires Internet connection or a working local web service)
  */
testCase1::testCase1(QWidget *parent) :
    QPushButton(parent)
{
    soapMessageTest();
}

testCase1::~testCase1()
{
    delete message;
}

void testCase1::messageResponse(QVariant msg)
{
    QString rply = msg.toString();
    qDebug() << rply;
}

void testCase1::soapMessageTest()
{
    /**
      Message parameters are specified here.
      */
    QMap<QString, QVariant> tmpP;
    tmpP.insert("symbol", QVariant("NOK"));

    /**
      This is the reply holder.
      */
    QMap<QString, QVariant> tmpR;
    tmpR.insert("GetQuoteResponse", QVariant("s:string"));

    message = new QWebServiceMethod("http://www.webservicex.net/stockquote.asmx", "GetQuote", tmpP, this);

    connect(message, SIGNAL(replyReady(QVariant)), this, SLOT(messageResponse(QVariant)));

    message->setTargetNamespace("http://www.webserviceX.NET/");
    message->sendMessage();
}
