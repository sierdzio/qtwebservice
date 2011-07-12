#ifndef FLAGS_H
#define FLAGS_H

#include "../../../QWebService_main/QWebService/headers/QWebService_global.h"

class Flags
{
public:
    enum Mode{fullMode, debugMode, compactMode};
    enum Synchronousness {synchronous, asynchronous};
    enum Structure {standardStructure, noMessagesStructure, allInOneDirStructure};
    enum BuildSystem {qmake, cmake, scons, noBuildSystem};

    Flags(Mode mode = fullMode,
          Synchronousness synchronousness = synchronous,
          Structure structure = standardStructure,
          QSoapMessage::Protocol protocol = QSoapMessage::soap12,
          BuildSystem = qmake, bool force = false);

    Mode mode;
    Synchronousness synchronousness;
    Structure structure;
    QSoapMessage::Protocol protocol;
    BuildSystem buildSystem;
    bool force;
};

#endif // FLAGS_H
