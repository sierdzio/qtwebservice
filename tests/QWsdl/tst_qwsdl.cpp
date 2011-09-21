/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the QWsdl test suite.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <qwsdl.h>

/*
    This test tests QWsdl operation.

    It does not require Internet connection - you can check a physical WSDL file
    if you want to, or specify address to where a remote WSDL resides.
  */
class TestQWsdl : public QObject
{
    Q_OBJECT

private slots:
    void initialTest();
    void gettersTest();
    void settersTest();
};

/*
  Performs basic checks of constructor and basic methods.
  */
void TestQWsdl::initialTest()
{
    QWsdl *wsdl1 = new QWsdl(this);
    QCOMPARE(wsdl1->isErrorState(), bool(false));
    delete wsdl1;

    QWsdl wsdl2(QString("../../examples/wsdl/band_ws.asmx"), this);
    QCOMPARE(wsdl2.isErrorState(), bool(false));

    QStringList tempList = wsdl2.methodNames();
    QCOMPARE(tempList.size(), int(13));
}

/*
  Performs basic checks of getters.
  */
void TestQWsdl::gettersTest()
{
    QWsdl wsdl(QString("../../examples/wsdl/band_ws.asmx"), this);

    QCOMPARE(wsdl.host(), QString("http://localhost:1304/band_ws.asmx"));
    QCOMPARE(wsdl.hostUrl(), QUrl("http://localhost:1304/band_ws.asmx"));
    QCOMPARE(wsdl.wsdlFile(), QString("../../examples/wsdl/band_ws.asmx"));
    QCOMPARE(wsdl.webServiceName(), QString("band_ws"));
    QCOMPARE(wsdl.targetNamespace(), QString("http://tempuri.org/"));
    QCOMPARE(wsdl.isErrorState(), bool(false));
    QCOMPARE(wsdl.errorInfo(), QString(""));
    QCOMPARE(wsdl.methodNames().size(), int(13));

    QStringList tempList = wsdl.methodNames();
    QMap<QString, QWebServiceMethod *> *methods = wsdl.methods();

    foreach (QString s, tempList) {
        QMap<QString, QVariant> tempParams = methods->value(s)->parameterNamesTypes();
        QMap<QString, QVariant> tempReturns = methods->value(s)->returnValueNameType();

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
void TestQWsdl::settersTest()
{
    QWsdl *wsdl;
    wsdl = new QWsdl(QString("../../examples/wsdl/band_ws.asmx"), this);
    QCOMPARE(wsdl->isErrorState(), bool(false));

    wsdl->resetWsdl(QString("../../examples/wsdl/LondonGoldFix.asmx.xml"));
    QCOMPARE(wsdl->isErrorState(), bool(false));
    QCOMPARE(wsdl->host(), QString("http://www.webservicex.net/LondonGoldFix.asmx"));
    QCOMPARE(wsdl->hostUrl(), QUrl("http://www.webservicex.net/LondonGoldFix.asmx"));
    QCOMPARE(wsdl->wsdlFile(), QString("../../examples/wsdl/LondonGoldFix.asmx.xml"));
    QCOMPARE(wsdl->webServiceName(), QString("LondonGoldAndSilverFix"));
    QCOMPARE(wsdl->targetNamespace(), QString("http://www.webservicex.net"));
    QCOMPARE(wsdl->errorInfo(), QString(""));
    QCOMPARE(wsdl->methodNames().size(), int(1));

    wsdl->setWsdlFile(QString("../../examples/wsdl/stockquote.asmx"));
    QCOMPARE(wsdl->isErrorState(), bool(false));
    QCOMPARE(wsdl->host(), QString("http://www.webservicex.net/stockquote.asmx"));
    QCOMPARE(wsdl->hostUrl(), QUrl("http://www.webservicex.net/stockquote.asmx"));
    QCOMPARE(wsdl->wsdlFile(), QString("../../examples/wsdl/stockquote.asmx"));
    QCOMPARE(wsdl->webServiceName(), QString("StockQuote"));
    QCOMPARE(wsdl->targetNamespace(), QString("http://www.webserviceX.NET/"));
    QCOMPARE(wsdl->errorInfo(), QString(""));
    QCOMPARE(wsdl->methodNames().size(), int(1));;

    delete wsdl;
}

QTEST_MAIN(TestQWsdl)
#include "tst_qwsdl.moc"

