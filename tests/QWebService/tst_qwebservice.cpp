/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the QWebService test suite.
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.txt included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#include <QtTest>
#include <qwebservice.h>

/**
  This test case checks both QWebService (and QWebService) functionality.
  */

class TestQWebService : public QObject
{
    Q_OBJECT

private slots:
    void initialTest();
    void gettersTest();
    void settersTest();
    void qpropertyTest();
    void methodManagementTest();
};

/*
  Performs basic checks of constructor and basic methods.
  */
void TestQWebService::initialTest()
{
    QWebService *reader1;
    reader1 = new QWebService(this);
    QCOMPARE(reader1->isErrorState(), bool(false));

    QWebService *reader2;
    reader2 = new QWebService(QString("../../../examples/wsdl/band_ws.asmx"), this);
    QCOMPARE(reader2->isErrorState(), bool(false));

    QWebService *reader3;
    QWsdl *wsdl = new QWsdl(QString("../../../examples/wsdl/band_ws.asmx"), this);
    reader3 = new QWebService(wsdl, this);
    QCOMPARE(reader3->isErrorState(), bool(false));

    delete reader1;
    delete reader2;
    delete reader3;
}

/*
  Performs basic checks of getters.
  */
void TestQWebService::gettersTest()
{
    QWebService reader(QString("../../../examples/wsdl/band_ws.asmx"), this);

    QCOMPARE(reader.host(), QString("../../../examples/wsdl/band_ws.asmx"));
    QCOMPARE(reader.hostUrl(), QUrl("../../../examples/wsdl/band_ws.asmx"));
    QCOMPARE(reader.isErrorState(), bool(false));
    QCOMPARE(reader.errorInfo(), QString(""));
    QCOMPARE(reader.methodNames().size(), int(13));

    QStringList tempList = reader.methodNames();

    foreach (QString s, tempList) {
        QMap<QString, QVariant> tempParams = reader.parameterNamesTypes(s);
        QMap<QString, QVariant> tempReturns = reader.returnValueNameType(s);

        if (s == QString("getBandName")) {
            QCOMPARE(tempParams.size(), int(1));
            QCOMPARE(tempReturns.size(), int(1));
        } else if (s == QString("getBandDescription")) {
            QCOMPARE(tempParams.size(), int(1));
            QCOMPARE(tempReturns.size(), int(1));
        } else if (s == QString("getBandsList")) {
            QCOMPARE(tempParams.size(), int(0));
            QCOMPARE(tempReturns.size(), int(1));
        } else if (s == QString("getBandsListForGenreAndDate")) {
            QCOMPARE(tempParams.size(), int(2));
            QCOMPARE(tempReturns.size(), int(1));
        } else if (s == QString("getBandsListForGenre")) {
            QCOMPARE(tempParams.size(), int(1));
            QCOMPARE(tempReturns.size(), int(1));
        } else if (s == QString("getGenreList")) {
            QCOMPARE(tempParams.size(), int(0));
            QCOMPARE(tempReturns.size(), int(1));
        } else if (s == QString("getBandPricePerShow")) {
            QCOMPARE(tempParams.size(), int(1));
            QCOMPARE(tempReturns.size(), int(1));
        } else if (s == QString("getBandsForADate")) {
            QCOMPARE(tempParams.size(), int(1));
            QCOMPARE(tempReturns.size(), int(1));
        } else if (s == QString("getNextEmptySlot")) {
            QCOMPARE(tempParams.size(), int(2));
            QCOMPARE(tempReturns.size(), int(1));
        } else if (s == QString("bookABand")) {
            QCOMPARE(tempParams.size(), int(11));
            QCOMPARE(tempReturns.size(), int(1));
        } else if (s == QString("cancelBookingById")) {
            QCOMPARE(tempParams.size(), int(1));
            QCOMPARE(tempReturns.size(), int(1));
        } else if (s == QString("cancelBookingById")) {
            QCOMPARE(tempParams.size(), int(3));
            QCOMPARE(tempReturns.size(), int(1));
        } else if (s == QString("getBandSchedule")) {
            QCOMPARE(tempParams.size(), int(1));
            QCOMPARE(tempReturns.size(), int(1));
        }
    }
}

/*
  Performs basic checks of setters.
  */
void TestQWebService::settersTest()
{
    QWebService *reader;
    reader = new QWebService(QString("../../../examples/wsdl/band_ws.asmx"), this);
    QCOMPARE(reader->isErrorState(), bool(false));

    reader->setHost(QString("../../../examples/wsdl/LondonGoldFix.asmx.xml"));
    QCOMPARE(reader->isErrorState(), bool(false));
    QCOMPARE(reader->host(), QString("../../../examples/wsdl/LondonGoldFix.asmx.xml"));
    QCOMPARE(reader->hostUrl(), QUrl("../../../examples/wsdl/LondonGoldFix.asmx.xml"));
    QCOMPARE(reader->errorInfo(), QString(""));
    QCOMPARE(reader->methodNames().size(), int(1));

    reader->setHost(QString("../../../examples/wsdl/stockquote.asmx"));
    QCOMPARE(reader->isErrorState(), bool(false));
    QCOMPARE(reader->host(), QString("../../../examples/wsdl/stockquote.asmx"));
    QCOMPARE(reader->hostUrl(), QUrl("../../../examples/wsdl/stockquote.asmx"));
    QCOMPARE(reader->errorInfo(), QString(""));
    QCOMPARE(reader->methodNames().size(), int(1));;

    delete reader;
}

/*
  Checks whether qproperties are working as intended.
  */
void TestQWebService::qpropertyTest()
{
    QWebService *reader = new QWebService(QString("../../../examples/wsdl/band_ws.asmx"), this);

    QCOMPARE(reader->property("name").toString(), QString("band_ws"));
    QCOMPARE(reader->property("host").toString(), QString("../../../examples/wsdl/band_ws.asmx"));
    QCOMPARE(QUrl(reader->property("hostUrl").toString()), QUrl("../../../examples/wsdl/band_ws.asmx"));
    QCOMPARE(reader->property("methodNames").toStringList().length(), int(13));

    delete reader;
}

/*
  Tests adding and removing web methods - boths by QWsdl and custom ones.
  */
void TestQWebService::methodManagementTest()
{
    QWebService *reader;
    reader = new QWebService(this);
    QCOMPARE(reader->isErrorState(), bool(false));

    reader->setWsdl(new QWsdl("../../../examples/wsdl/band_ws.asmx", reader));
    QCOMPARE(reader->isErrorState(), bool(false));
    QCOMPARE(reader->methodNames().size(), int(13));
    reader->resetWsdl();
    QCOMPARE(reader->isErrorState(), bool(false));
    QCOMPARE(reader->methodNames().size(), int(0));

    reader->addMethod("testMethod1", new QWebServiceMethod(/*reader*/));
    QCOMPARE(reader->isErrorState(), bool(false));
    QCOMPARE(reader->methodNames().size(), int(1));
    reader->removeMethod("testMethod1");
    QCOMPARE(reader->isErrorState(), bool(false));
    QCOMPARE(reader->methodNames().size(), int(0));

    delete reader;
}

QTEST_MAIN(TestQWebService)
#include "tst_qwebservice.moc"
