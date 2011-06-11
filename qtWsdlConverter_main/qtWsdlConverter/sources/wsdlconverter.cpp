#include "../headers/wsdlconverter.h"

WsdlConverter::WsdlConverter(QString wsdlFileOrUrl, QObject *parent, QDir outputDir, QString baseOutputClsNme) :
    QObject(parent)
{
    wsdl = new QWsdl(wsdlFileOrUrl, this);
    errorState = false;
    errorMessage = "";

    loadMessages();
}

WsdlConverter::~WsdlConverter()
{
    delete wsdl;
}

void WsdlConverter::loadMessages()
{
    messages = wsdl->getMethods();
}
