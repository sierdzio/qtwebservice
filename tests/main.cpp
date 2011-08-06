#include <QtCore>
#include <QtGui/QApplication>
#include <QtGui/QPushButton>
#include "../QWebService/headers/qwsdl.h"
#include "testCase1.h"
#include "testcase2_wsdl.h"
#include "testcase3_webservice.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    testCase1 tempButton;
//    testCase2_wsdl tempButton;
    testCase3_webservice tempButton;

    tempButton.setGeometry(100, 200, 200, 100);
    tempButton.setText("CLICK ME!");

    QObject::connect(&tempButton, SIGNAL(clicked()), &a, SLOT(quit()));
    tempButton.show();

    return a.exec();
}
