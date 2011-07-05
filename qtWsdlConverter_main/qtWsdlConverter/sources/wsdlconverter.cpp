#include "../headers/wsdlconverter.h"

WsdlConverter::WsdlConverter(QString wsdlFileOrUrl, QObject *parent, QDir outputDir, QString baseOutputClsNme) :
    QObject(parent)
{
    errorState = false;
    errorMessage = "";
    wsdl = new QWsdl(wsdlFileOrUrl, this);

    if (wsdl->isErrorState())
        enterErrorState("WSDL error!");
    else
    {
        messages = wsdl->getMethods();
    }

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
    /*
      Algorithm - initial plan:
      1. Load messages.
      2. Check and create the directory.
      3. (optional, for new flags) Dive into specified code creation path.
      4. (for standard path) Create dirs 'headers' and 'sources'.
      5. Create headers for messages (yes, a big task in one point :) ).
      6. Create sources for messages.
      7. Create the QWebServiceReaderAbstract subclass's header and source, put it into
            suitable directory.
      8. Create <webServiceName>.pro file.

      */

    loadMessages();
}

void WsdlConverter::loadMessages()
{
    messages = wsdl->getMethods();
}

QString WsdlConverter::getWebServiceName()
{
    return wsdl->getWebServiceName();
}
