#ifndef FLAGS_H
#define FLAGS_H

#include "../../../QWebService_main/QWebService/headers/QWebService_global.h"

class Flags
{
public:
    enum Mode{fullMode, debugMode, compactMode};
    enum Synchronousness {synchronous, asynchronous};
    enum Structure {standardStructure, noMessagesStructure, allInOneDirStructure};

    Flags(Mode mode = fullMode,
          Synchronousness synchronousness = asynchronous,
          Structure structure = standardStructure,
          QSoapMessage::Protocol protocol = QSoapMessage::soap12);

//    int mode();
//    int synchronousness();
//    int structure();
//    int protocol();

//    void setMode(Flags::Mode mode);
//    void setSynchronousness(Flags::Synchronousness synchronousness);
//    void setStructure(Flags::Structure structure);
//    void setProtocol(QSoapMessage::Protocol protocol);

//private:
    Mode mode;
    Synchronousness synchronousness;
    Structure structure;
    QSoapMessage::Protocol protocol;
};

#endif // FLAGS_H
