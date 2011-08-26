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
    void gettersTest();
    void settersTest();
    void asynchronousSendingTest();
};

/*
  Performs basic checks of constructor and basic methods.
  */
void TestQWebMethod::initialTest()
{
    QWebMethod *message = new QWebMethod(0, QWebMethod::soap12, QWebMethod::POST);
    QCOMPARE(message->isErrorState(), bool(false));

    delete message;
}

/*
  Performs basic checks of getters.
  */
void TestQWebMethod::gettersTest()
{
    QWebMethod *message = new QWebMethod(0, QWebMethod::soap12, QWebMethod::POST);
    QCOMPARE(message->isErrorState(), bool(false));
    QCOMPARE(message->isReplyReady(), bool(false));
    QCOMPARE(message->errorInfo(), QString(""));
    QCOMPARE(message->httpMethodString(), QString("POST"));
    QCOMPARE(message->httpMethod(), QWebMethod::POST);
    QCOMPARE(message->protocolString(false), QString("soap12"));
    QCOMPARE(message->protocolString(true), QString("soap12"));
    QCOMPARE(message->protocol(), QWebMethod::soap12);
    QCOMPARE(message->hostUrl(), QUrl(""));
    QCOMPARE(message->host(), QString(""));
    QCOMPARE(message->targetNamespace(), QString(""));
    QCOMPARE(message->messageName(), QString(""));

    /*
    QStringList parameterNames() const;
    QStringList returnValueName() const;
    QMap<QString, QVariant> parameterNamesTypes() const;
    QMap<QString, QVariant> returnValueNameType() const;
    */
    delete message;
}

/*
  Performs basic checks of setters.
  */
void TestQWebMethod::settersTest()
{
    /*
    void setHost(QString newHost);
    void setHost(QUrl newHost);
    void setMessageName(QString newName);
    void setParameters(QMap<QString, QVariant> params);
    void setReturnValue(QMap<QString, QVariant> returnValue);
    void setTargetNamespace(QString tNamespace);
    void setProtocol(Protocol protocol);
    void setHttpMethod(HttpMethod method);
    */
}

/*
  Checks QWebMethod operation when using the default sendMessage(QByteArray)
  */
void TestQWebMethod::asynchronousSendingTest()
{
    /*
    bool sendMessage(QByteArray requestData = QByteArray());
    QVariant replyRead();
    */
}

QTEST_MAIN(TestQWebMethod)
#include "tst_qwebmethod.moc"
