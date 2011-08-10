#include <QtCore>
#include <QtCore/QCoreApplication>
#include "../headers/wsdlconverter.h"

/**
  *
  */
int main(int argc, char *argv[])
{    
    QCoreApplication a(argc, argv);

    WsdlConverter converter(qApp->arguments());
    converter.convert();

    return 0; //a.exec();
}
