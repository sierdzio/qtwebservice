#include <QtTest/QtTest>
#include <qwsdl.h>

/*
  This test tests QWsdl operation.

  It does not require Internet connection - you can check a physical WSDL file if you want to,
  or specify address to where a remote WSDL resides.
  */

class TestQWsdl : public QObject
{
    Q_OBJECT

private slots:
    void initialTest();
};

void TestQWsdl::initialTest()
{
    QWsdl wsdl("../../examples/band_ws.asmx", this);
    QCOMPARE(wsdl.isErrorState(), bool(false));

    QStringList tempList = wsdl.methodNames();
    QCOMPARE(tempList.size(), int(13));
}

QTEST_MAIN(TestQWsdl)
#include "tst_qwsdl.moc"

