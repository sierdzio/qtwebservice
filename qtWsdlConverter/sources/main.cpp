#include <QtCore/qcoreapplication.h>
#include "../headers/wsdlconverter.h"

/**
  *
  */
int main(int argc, char *argv[])
{    
    QCoreApplication a(argc, argv);

    WsdlConverter converter(a.arguments());
    if (!converter.isErrorState()) {
        converter.convert();
    }

    if (converter.isErrorState())
        printf(converter.errorInfo().toLocal8Bit());

    return 0;//    a.exec();
}
