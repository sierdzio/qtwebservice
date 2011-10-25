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
    QWebServiceMethod *method = new QWebServiceMethod(QWebMethod::Soap12, QWebMethod::Post, 0);
    QCOMPARE(method->isErrorState(), bool(false));

    delete method;
}

/*
  Performs basic checks of getters.
  */
void TestQWebServiceMethod::gettersTest()
{
    QWebServiceMethod *method = new QWebServiceMethod(QWebMethod::Soap12, QWebMethod::Post, 0);
    defaultGettersTest(method);
    delete method;
}

/*
  Performs basic checks of setters.
  */
void TestQWebServiceMethod::settersTest()
{
    QWebServiceMethod *method = new QWebServiceMethod(QWebMethod::Soap12, QWebMethod::Post, 0);
    defaultGettersTest(method);

    QUrl tempUrl("http://www.currencyserver.de/webservice/currencyserverwebservice.asmx");
    method->setHost(tempUrl);
    QCOMPARE(method->hostUrl(), tempUrl);

    QString tempName = "getProviderList";
    method->setMethodName(tempName);
    QCOMPARE(method->methodName(), tempName);

    QString tempTargetNmspc = "http://www.daenet.de/webservices/CurrencyServer";
    method->setTargetNamespace(tempTargetNmspc);
    QCOMPARE(method->targetNamespace(), tempTargetNmspc);

    method->setProtocol(QWebMethod::Json);
    QCOMPARE(method->protocol(), QWebMethod::Json);
    QCOMPARE(method->protocolString(), QString("Json"));
    QCOMPARE(method->protocolString(true), QString("Json"));

    method->setHttpMethod(QWebMethod::Delete);
    QCOMPARE(method->httpMethod(), QWebMethod::Delete);
    QCOMPARE(method->httpMethodString(), QString("Delete"));

    method->setHttpMethod("get");
    QCOMPARE(method->httpMethod(), QWebMethod::Get);
    QCOMPARE(method->httpMethodString(), QString("Get"));

    method->setHttpMethod("pUT");
    QCOMPARE(method->httpMethod(), QWebMethod::Put);
    QCOMPARE(method->httpMethodString(), QString("Put"));

    method->setHttpMethod("POST");
    QCOMPARE(method->httpMethod(), QWebMethod::Post);
    QCOMPARE(method->httpMethodString(), QString("Post"));

    QMap<QString, QVariant> tmpP;
    tmpP.insert("symbol", QVariant("NOK"));
    method->setParameters(tmpP);
    QCOMPARE(method->parameterNames().size(), int(1));
    QCOMPARE(method->parameterNames().first(), QString("symbol"));
    QCOMPARE(method->parameterNamesTypes().value("symbol"), QVariant("NOK"));

    method->setReturnValue(tmpP);
    QCOMPARE(method->returnValueName().size(), int(1));
    QCOMPARE(method->returnValueName().first(), QString("symbol"));
    QCOMPARE(method->returnValueNameType().value("symbol"), QVariant("NOK"));

    delete method;
}

/*
  This test requires Internet connection!
  */
void TestQWebServiceMethod::asynchronousTest()
{
    // method parameters are specified here.
    QMap<QString, QVariant> tmpP;
    QWebServiceMethod *method = new
            QWebServiceMethod("http://www.currencyserver.de/webservice/currencyserverwebservice.asmx",
                              "getProviderList",
                              QWebMethod::Soap12, QWebMethod::Post, this);
    method->setTargetNamespace("http://www.daenet.de/webservices/CurrencyServer");
    method->invokeMethod(tmpP);

    QCOMPARE(method->isErrorState(), bool(false));

    bool result = false;
    for (int i = 0; (i < 50) && (!method->isReplyReady()); i++)
        QTest::qWait(250);

    result = method->isReplyReady();

    QCOMPARE(result, bool(true));
    delete method;
}

/*
  This test requires Internet connection!
  */
void TestQWebServiceMethod::synchronousTest()
{
    // method parameters are specified here.
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

void TestQWebServiceMethod::defaultGettersTest(QWebServiceMethod *method)
{
    QCOMPARE(method->isErrorState(), bool(false));
    QCOMPARE(method->isReplyReady(), bool(false));
    QCOMPARE(method->errorInfo(), QString(""));
    QCOMPARE(method->httpMethodString(), QString("Post"));
    QCOMPARE(method->httpMethod(), QWebMethod::Post);
    QCOMPARE(method->protocolString(false), QString("Soap12"));
    QCOMPARE(method->protocolString(true), QString("Soap12"));
    QCOMPARE(method->protocol(), QWebMethod::Soap12);
    QCOMPARE(method->hostUrl(), QUrl(""));
    QCOMPARE(method->host(), QString(""));
    QCOMPARE(method->targetNamespace(), QString(""));
    QCOMPARE(method->methodName(), QString(""));
    QCOMPARE(method->returnValueNameType().size(), int(0));
    QCOMPARE(method->returnValueName().size(), int(0));
    QCOMPARE(method->parameterNamesTypes().size(), int(0));
    QCOMPARE(method->parameterNames().size(), int(0));
}

QTEST_MAIN(TestQWebServiceMethod)
#include "tst_qwebservicemethod.moc"
