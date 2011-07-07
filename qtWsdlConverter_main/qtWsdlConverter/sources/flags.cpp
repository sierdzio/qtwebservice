#include "../headers/flags.h"

Flags::Flags(Mode mod,
             Synchronousness synchronousnes,
             Structure structur,
             QSoapMessage::Protocol protoco,
             BuildSystem system) :
    mode(mod), synchronousness(synchronousnes), structure(structur), protocol(protoco), buildSystem(system)
{
}
