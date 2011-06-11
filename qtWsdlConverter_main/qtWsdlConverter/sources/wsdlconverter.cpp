#include "../headers/wsdlconverter.h"

WsdlConverter::WsdlConverter(QString wsdlFileOrUrl, QObject *parent) :
    QObject(parent)
{
    wsdl = new QWsdl(wsdlFileOrUrl, this);
    errorState = false;
    errorMessage = "";
}

WsdlConverter::~WsdlConverter()
{
    delete wsdl;
}
