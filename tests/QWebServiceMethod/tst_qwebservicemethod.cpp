/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the QWebServiceMethod test suite.
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.txt included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#include <QtTest/QtTest>
#include <qwebservicemethod.h>

/**
    This test checks QWebServiceMethod in operation
    (requires Internet connection or a working local web service)
  */
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
    QWebServiceMethod *message = new QWebServiceMethod(QWebMethod::Soap12, QWebMethod::Post, 0);
    QCOMPARE(message->isErrorState(), bool(false));

    delete message;
}

/*
  Performs basic checks of getters.
  */
void TestQWebServiceMethod::gettersTest()
{
    QWebServiceMethod *message = new QWebServiceMethod(QWebMethod::Soap12, QWebMethod::Post, 0);
    defaultGettersTest(message);
    delete message;
}

/*
  Performs basic checks of setters.
  */
void TestQWebServiceMethod::settersTest()
{
    QWebServiceMethod *message = new QWebServiceMethod(QWebMethod::Soap12, QWebMethod::Post, 0);
    defaultGettersTest(message);

    QUrl tempUrl("http://www.currencyserver.de/webservice/currencyserverwebservice.asmx");
    message->setHost(tempUrl);
    QCOMPARE(message->hostUrl(), tempUrl);

    QString tempName = "getProviderList";
    message->setMethodName(tempName);
    QCOMPARE(message->methodName(), tempName);

    QString tempTargetNmspc = "http://www.daenet.de/webservices/CurrencyServer";
    message->setTargetNamespace(tempTargetNmspc);
    QCOMPARE(message->targetNamespace(), tempTargetNmspc);

    message->setProtocol(QWebMethod::Json);
    QCOMPARE(message->protocol(), QWebMethod::Json);
    QCOMPARE(message->protocolString(), QString("Json"));
    QCOMPARE(message->protocolString(true), QString("Json"));

    message->setHttpMethod(QWebMethod::Delete);
    QCOMPARE(message->httpMethod(), QWebMethod::Delete);
    QCOMPARE(message->httpMethodString(), QString("Delete"));

    message->setHttpMethod("get");
    QCOMPARE(message->httpMethod(), QWebMethod::Get);
    QCOMPARE(message->httpMethodString(), QString("Get"));

    message->setHttpMethod("pUT");
    QCOMPARE(message->httpMethod(), QWebMethod::Put);
    QCOMPARE(message->httpMethodString(), QString("Put"));

    message->setHttpMethod("POST");
    QCOMPARE(message->httpMethod(), QWebMethod::Post);
    QCOMPARE(message->httpMethodString(), QString("Post"));

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
                              QWebMethod::Soap12, QWebMethod::Post, this);
    message->setTargetNamespace("http://www.daenet.de/webservices/CurrencyServer");
    message->invokeMethod(tmpP);

    QCOMPARE(message->isErrorState(), bool(false));

    bool result = false;
    for (int i = 0; (i < 50) && (!message->isReplyReady()); i++)
        QTest::qWait(250);

    result = message->isReplyReady();

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
    reply = QWebServiceMethod::invokeMethod(
             QUrl("http://www.currencyserver.de/webservice/currencyserverwebservice.asmx"),
             "getProviderList", "http://www.daenet.de/webservices/CurrencyServer",
             tmpP, QWebMethod::Soap12, QWebMethod::Post, this);

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
    QCOMPARE(message->httpMethodString(), QString("Post"));
    QCOMPARE(message->httpMethod(), QWebMethod::Post);
    QCOMPARE(message->protocolString(false), QString("Soap12"));
    QCOMPARE(message->protocolString(true), QString("Soap12"));
    QCOMPARE(message->protocol(), QWebMethod::Soap12);
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
