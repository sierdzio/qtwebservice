#include "../headers/wsdlconverter.h"

WsdlConverter::WsdlConverter(QString wsdlFileOrUrl, QObject *parent, QDir outputDir, QString baseOutputClsNme) :
    QObject(parent)
{
    errorState = false;
    errorMessage = "";
    wsdl = new QWsdl(wsdlFileOrUrl, this);

    if (wsdl->isErrorState())
        enterErrorState("WSDL error!");

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

bool WsdlConverter::isErrorState()
{
    return errorState;
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

void WsdlConverter::convert()
{
    loadMessages();
}

void WsdlConverter::loadMessages()
{
    messages = wsdl->getMethods();
}
