/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the QWebMethod test suite.
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
#include <qwebmethod.h>

/**
  This test checks QWebMethod in operation (requires Internet connection or a working local web service)
  */
class TestQWebMethod : public QObject
{
    Q_OBJECT

private slots:
    void initialTest();
    void gettersTest();
    void settersTest();
    void asynchronousSendingTest();

private:
    void defaultGettersTest(QWebMethod *msg);
};

/*
  Performs basic checks of constructor and basic methods.
  */
void TestQWebMethod::initialTest()
{
    QWebMethod *message = new QWebMethod(0, QWebMethod::Soap12, QWebMethod::Post);
    QCOMPARE(message->isErrorState(), bool(false));
    delete message;
}

/*
  Performs basic checks of getters.
  */
void TestQWebMethod::gettersTest()
{
    QWebMethod *message = new QWebMethod(0, QWebMethod::Soap12, QWebMethod::Post);
    defaultGettersTest(message);
    delete message;
}

/*
  Performs basic checks of setters.
  */
void TestQWebMethod::settersTest()
{
    QWebMethod *message = new QWebMethod(0, QWebMethod::Soap12, QWebMethod::Post);
    defaultGettersTest(message);

    QUrl tempUrl(QString("http://www.currencyserver.de/webservice/currencyserverwebservice.asmx"));
    message->setHost(tempUrl);
    QCOMPARE(message->hostUrl(), tempUrl);

    QString tempName("getProviderList");
    message->setMethodName(tempName);
    QCOMPARE(message->methodName(), tempName);

    QString tempTargetNmspc("http://www.daenet.de/webservices/CurrencyServer");
    message->setTargetNamespace(tempTargetNmspc);
    QCOMPARE(message->targetNamespace(), tempTargetNmspc);

    message->setProtocol(QWebMethod::Json);
    QCOMPARE(message->protocol(), QWebMethod::Json);
    QCOMPARE(message->protocolString(), QString("json"));
    QCOMPARE(message->protocolString(true), QString("json"));

    message->setHttpMethod(QWebMethod::Delete);
    QCOMPARE(message->httpMethod(), QWebMethod::Delete);
    QCOMPARE(message->httpMethodString(), QString("DELETE"));

    message->setHttpMethod("get");
    QCOMPARE(message->httpMethod(), QWebMethod::Get);
    QCOMPARE(message->httpMethodString(), QString("GET"));

    message->setHttpMethod("pUT");
    QCOMPARE(message->httpMethod(), QWebMethod::Put);
    QCOMPARE(message->httpMethodString(), QString("PUT"));

    message->setHttpMethod("POST");
    QCOMPARE(message->httpMethod(), QWebMethod::Post);
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
  Checks QWebMethod operation when using the default sendMessage(QByteArray)
  */
void TestQWebMethod::asynchronousSendingTest()
{
    QWebMethod *message = new QWebMethod(0, QWebMethod::Soap12, QWebMethod::Post);
    message->setHost("http://www.currencyserver.de/webservice/currencyserverwebservice.asmx");
    message->setMethodName("getProviderList");
    message->setTargetNamespace("http://www.daenet.de/webservices/CurrencyServer");
    message->invokeMethod();
    QCOMPARE(message->isErrorState(), bool(false));

    bool result = false;
    for (int i = 0; (i < 50) && (!message->isReplyReady()); i++)
        QTest::qWait(250);

    result = message->isReplyReady();

    QCOMPARE(result, bool(true));

    delete message;
}

void TestQWebMethod::defaultGettersTest(QWebMethod *message)
{
    QCOMPARE(message->isErrorState(), bool(false));
    QCOMPARE(message->isReplyReady(), bool(false));
    QCOMPARE(message->errorInfo(), QString(""));
    QCOMPARE(message->httpMethodString(), QString("POST"));
    QCOMPARE(message->httpMethod(), QWebMethod::Post);
    QCOMPARE(message->protocolString(false), QString("soap12"));
    QCOMPARE(message->protocolString(true), QString("soap12"));
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

QTEST_MAIN(TestQWebMethod)
#include "tst_qwebmethod.moc"
