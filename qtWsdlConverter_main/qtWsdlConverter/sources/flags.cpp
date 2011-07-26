#include "../headers/flags.h"

/*!
    \class Flags
    \brief Holds all important flags used by converter.

    Currently, every setting is held in a different enum, and all variables are public.
    In the future, flags will probably be unified into one/ two enums, thus enabling easy
    OR switching.
  */

/*!
    \fn Flags::Flags(Mode mod, Synchronousness synchronousnes, Structure structur, QSoapMessage::Protocol protoco, BuildSystem system, bool forc)

    Constructs the Flags object. All params are optional.
    Default values:
    \list
    \o \a mod = fullMode
    \o \a synchronousnes = synchronous
    \o \a structur = standardStructure
    \o \a protoco = QSoapMessage::soap12
    \o \a system = qmake
    \o \a forc = false
    \endlist
  */
Flags::Flags(Mode mod,
             Synchronousness synchronousnes,
             Structure structur,
             QSoapMessage::Protocol protoco,
             BuildSystem system,
             bool forc) :
    mode(mod), synchronousness(synchronousnes), structure(structur), protocol(protoco), buildSystem(system),
    force(forc)
{
}
