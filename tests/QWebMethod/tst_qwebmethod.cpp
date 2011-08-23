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
    void initialTests();
};

void TestQWebMethod::initialTests()
{
    QWebMethod *message = new QWebMethod(0, QWebMethod::soap12, QWebMethod::POST);

    QCOMPARE(message->isErrorState(), bool(false));

    delete message;
}

QTEST_MAIN(TestQWebMethod)
#include "tst_qwebmethod.moc"
