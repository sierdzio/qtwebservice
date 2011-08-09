#include <QtCore>
#include <QtGui/QApplication>
#include <QtGui/QPushButton>
#include "testCase1.h"
#include "testcase2_wsdl.h"
#include "testcase3_webservice.h"

/**
  This is used to invoke test cases.

  Test cases, at least currently, are QButton derivatives, which adds a simple way of
  terminating the program - just click the button.
  */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*
      Test cases to choose from:
      */
//    testCase1 tempButton;
//    testCase2_wsdl tempButton;
    testCase3_webservice tempButton;

    tempButton.setGeometry(100, 200, 200, 100);
    tempButton.setText("CLICK ME!");

    QObject::connect(&tempButton, SIGNAL(clicked()), &a, SLOT(quit()));
    tempButton.show();

    return a.exec();
}
