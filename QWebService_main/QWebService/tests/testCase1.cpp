#include "testCase1.h"

testCase1::testCase1(QWidget *parent) :
    QPushButton(parent)
{
    QMap<QString, QVariant> tmpP;
    tmpP.insert("symbol", QVariant("NOK"));

    QMap<QString, QVariant> tmpR;
    tmpP.insert("GetQuoteResponse", QVariant("s:string"));

//    message = new QSoapMessage(QUrl("http://www.webservicex.net/stockquote.asmx"),
//                                       "GetQuote", this);
    message = new QSoapMessage("http://www.webservicex.net/stockquote.asmx", "GetQuote", tmpP, tmpR, this);

    connect(message, SIGNAL(replyReady(QVariant)), this, SLOT(messageResponse(QVariant)));

//    message->setParams(tmpP, tmpR);
//    message->setTargetNamespace("http://www.webservicex.net/stockquote.asmx");
    message->sendMessage();
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
