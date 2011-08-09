#include "../headers/flags.h"

/*!
    \class Flags
    \brief Holds all important flags used by converter.

    All application's switches are unified thus enabling easy
    OR switching. The only exception is th e--force switch, which is held as a separate
    bool variable.
  */

/*!
     \enum Flags::Option

     This enum type specifies code creation mode:

        Mode:
     \value fullMode
            All enums and variables are copied, enabling some flexibility in use.
     \value debugMode
            fullMode + debug messages.
     \value compactMode
            Only most needed methods, variables and enums are preserved. Code is as small, as possible, at the expense of loosing flexibility.
     \value synchronous
            Web methods will wait for reply and return with data.
     \value asynchronous
            Web methods will return instantly. Data availability will be announced by emitting a signal.
     \value standardStructure
            Headers will be placed in <dir>/headers, sources in <dir>/sources, build system and .pro files in <dir>/.
     \value noMessagesStructure
            Converter will not create messages as separate classes, but use QSoapMessage.
     \value allInOneDirStructure
            All generated files will be stored in one folder.
     \value qmake
            qmake will be used.
     \value cmake
            cmake will be used.
     \value scons
            scons will be used.
     \value noBuildSystem
            No build system files will be created.
     \value http
            http protocol will be used.
     \value soap10
            SOAP 1.0 protocol will be used.
     \value soap12
            SOAP 1.2 protocol will be used.
     \value soap
            Internall wrapper for all SOAP protocols. When used in app, will default to SOAP 1.2
     \value json
            JSON protocol will be used.
 */

/*!
    \fn Flags::Flags(Options options, bool forced)

    Constructs the Flags object. All params are optional.
    Default values:
    \list
    \o \a options :
    \list
        \o fullMode
        \o synchronous
        \o standardStructure
        \o soap12
        \o qmake
    \endlist
    \o \a forced = false
    \endlist
  */
Flags::Flags(Options options, bool forced)
{
    this->options = options;
    this->force = forced;
}

/*!
    \fn Flags::resetFlags()

    Resets all flags to default values. This does not apply to '--forced'.
  */
void Flags::resetFlags()
{
    options = 0;
    options = fullMode | synchronous | standardStructure | qmake | soap12;
}

/*!
    \fn Flags::resetFlags(Options whatToReset)

    Resets (zeroes) flags set to 1 in \a whatToReset. This does not apply to '--forced'.
  */
void Flags::resetFlags(Options whatToReset)
{
    this->options &= ~whatToReset;
}

/*!
    \fn Flags::setFlags(Options options)

    Does OR combination of \a options with those already set.
  */
void Flags::setFlags(Options options)
{
    Options changed = (options ^ this->options);

    if (!changed)
            return;

    this->options |= options;
}

/*!
    \fn Flags::setForced(bool forced)

    Sets the \a forced value to given one.

    When 'true', converter will delete old sources, and create
    a fresh copy every time it is run.
  */
void Flags::setForced(bool forced)
{
    this->force = forced;
}

/*!
    \fn Flags::flags() const

    Returns currently set options enum.
  */
Flags::Options Flags::flags() const
{
    return this->options;
}

/*!
    \fn Flags::forced() const

    Same as isForced().

    Returns force state.

    \sa isForced()
  */
bool Flags::forced() const
{
    return isForced();
}

/*!
    \fn Flags::isForced() const

    Same as forced().

    Returns force state.

    \sa forced()
  */
bool Flags::isForced() const
{
    return this->force;
}
