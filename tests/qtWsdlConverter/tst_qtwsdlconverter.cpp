/**
  This test checks converter in operation (requires Internet connection or a working local web service)
  */

#include <QtTest/QtTest>

class TestConverter : public QObject
{
    Q_OBJECT

private slots:
    void standardConversion();
};

void TestConverter::standardConversion()
{
    ;
}

QTEST_MAIN(TestConverter)
#include "tst_qtwsdlconverter.moc"

