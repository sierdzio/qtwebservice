#include "../headers/flags.h"

/*!
    \class Flags
    \brief Holds all important flags used by converter.

    Currently, every setting is held in a different enum, and all variables are public.
    In the future, flags will probably be unified into one/ two enums, thus enabling easy
    OR switching.
  */

/*!
     \enum Flags::Mode

     This enum type specifies code creation mode:

     \value fullMode
            All enums and variables are copied, enabling some flexibility in use.
     \value debugMode
            fullMode + debug messages.
     \value compactMode
            Only most needed methods, variables and enums are preserved. Code is as small, as possible, at the expense of loosing flexibility.
 */

/*!
     \enum Flags::Synchronousness

     This enum type specifies the way messages will be sent and received:

     \value synchronous
            Web methods will wait for reply and return with data.
     \value asynchronous
            Web methods will return instantly. Data availability will be announced by emitting a signal.
 */

/*!
     \enum Flags::Structure

     This enum type specifies the filesystem structure (the way files will be written on disk):

     \value standardStructure
            Headers will be placed in <dir>/headers, sources in <dir>/sources, build system and .pro files in <dir>/.
     \value noMessagesStructure
            Converter will not create messages as separate classes, but use QSoapMessage.
     \value allInOneDirStructure
            All generated files will be stored in one folder.
 */

/*!
     \enum Flags::BuildSystem

     This enum type specifies the build system to use:

     \value qmake
            qmake will be used.
     \value cmake
            cmake will be used.
     \value scons
            scons will be used.
     \value noBuildSystem
            No build system files will be created.
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
