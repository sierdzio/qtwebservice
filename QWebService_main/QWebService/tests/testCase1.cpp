#include "testCase1.h"

testCase1::testCase1(QWidget *parent) :
    QPushButton(parent)
{
    soapMessageTest();
}

testCase1::~testCase1()
{
    delete wsdl;
    delete message;
}

void testCase1::messageResponse(QVariant msg)
{
    QString rply = msg.toString();
    qDebug() << rply;
}

void testCase1::soapMessageTest()
{
    QMap<QString, QVariant> tmpP;
    tmpP.insert("symbol", QVariant("NOK"));

    QMap<QString, QVariant> tmpR;
    tmpR.insert("GetQuoteResponse", QVariant("s:string"));

    message = new QSoapMessage("http://www.webservicex.net/stockquote.asmx", "GetQuote", tmpP, tmpR, this);

    connect(message, SIGNAL(replyReady(QVariant)), this, SLOT(messageResponse(QVariant)));

    message->setTargetNamespace("http://www.webserviceX.NET/");
    message->sendMessage();
}
