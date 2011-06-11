#include <QtCore>
#include <QtCore/QCoreApplication>
#include "../headers/wsdlconverter.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    WsdlConverter converter("/home/sierdzio/Dropbox/Code/QWebService/QWebService_main/QWebService/examples/band_ws.asmx");

    return a.exec();
}
