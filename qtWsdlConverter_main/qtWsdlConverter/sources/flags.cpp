#include "../headers/flags.h"

Flags::Flags(Mode mod, Synchronousness synchronousnes, Structure structur, QSoapMessage::Protocol protoco) :
    mode(mod), synchronousness(synchronousnes), structure(structur), protocol(protoco)
{
}
/*
int Flags::mode()
{
    return mod;
}

int Flags::synchronousness()
{
    return synchro;
}

int Flags::structure()
{
    return struc;
}

QSoapMessage::Protocol Flags::protocol()
{
    return prot;
}

void Flags::setMode(Flags::Mode mode)
{
    mod = mod;
}

void Flags::setSynchronousness(Flags::Synchronousness synchronousness)
{
    synchro = synchronousness;
}

void Flags::setStructure(Flags::Structure structure)
{
    struc = structure;
}

void Flags::setProtocol(QSoapMessage::Protocol protocol)
{
    prot = protocol;
}
*/
