#include "../headers/wsdlconverter.h"

WsdlConverter::WsdlConverter(QString wsdlFileOrUrl, QObject *parent, QDir outputDir, QString baseOutputClsNme) :
    QObject(parent)
{
    wsdl = new QWsdl(wsdlFileOrUrl, this);
    errorState = false;
    errorMessage = "";

    // Setting default flags:
    synchronousness = synchronous;
    protocol = QSoapMessage::soap12;
}

WsdlConverter::~WsdlConverter()
{
    delete wsdl;
}

void WsdlConverter::setFlags(WsdlConverter::Synchronousness synch, QSoapMessage::Protocol prot)
{
    synchronousness = synch;
    protocol = prot;
    setFlagsOnMessages();
}

void WsdlConverter::convert()
{
    loadMessages();
}

bool WsdlConverter::isErrorState()
{
    return errorState;
}

void WsdlConverter::loadMessages()
{
    messages = wsdl->getMethods();
}

void WsdlConverter::setFlagsOnMessages()
{
    foreach (QString s, messages->keys())
    {
        messages->value(s)->setProtocol(protocol);
    }
}

void WsdlConverter::enterErrorState(QString errMessage)
{
    errorState = true;
    errorMessage = errMessage;
    emit errorEncountered(errMessage);
}
