#include "../headers/flags.h"

/*!
    \class Flags
    \brief Holds all important flags used by converter.

    All application's switches are unified thus enabling easy
    OR switching. The only exception is the --force switch, which is held as a separate
    bool variable.

    All entries in single sections (with an exception of build system related switches) are exclusive - you can use only one at a time (the lates one specified is used). For example, if you use --http and --json, only JSON flag will be set. Sections are specified in enum documentation (the table below).
  */

/*!
     \enum Flags::Option

     This enum type specifies code creation mode. Sections are indicated in brackets:

        Mode:
     \value fullMode
            (mode) All enums and variables are copied, enabling some flexibility in use.
     \value debugMode
            (mode) fullMode + debug messages.
     \value compactMode
            (mode) Only most needed methods, variables and enums are preserved. Code is as small, as possible, at the expense of loosing flexibility.
     \value synchronous
            (synchronousness) Web methods will wait for reply and return with data.
     \value asynchronous
            (synchronousness) Web methods will return instantly. Data availability will be announced by emitting a signal.
     \value standardStructure
            (structure) Headers will be placed in <dir>/headers, sources in <dir>/sources, build system and .pro files in <dir>/.
     \value noMessagesStructure
            (structure) Converter will not create messages as separate classes, but use QWebMethod.
     \value allInOneDirStructure
            (structure) All generated files will be stored in one folder.
     \value qmake
            (build system) qmake will be used.
     \value cmake
            (build system) cmake will be used.
     \value scons
            (build system) scons will be used.
     \value noBuildSystem
            (build system) No build system files will be created.
     \value http
            (protocol) http protocol will be used.
     \value soap10
            (protocol) SOAP 1.0 protocol will be used.
     \value soap12
            (protocol) SOAP 1.2 protocol will be used.
     \value soap
            (protocol) Internall wrapper for all SOAP protocols. When used in app, will default to SOAP 1.2
     \value json
            (protocol) JSON protocol will be used.
 */

/*!
    \fn Flags::Flags(Options options, bool forced)

    Constructs the Flags object. All params are optional. If you want to set non-default suffixes, use setObjSuffix() and setMsgSuffix().

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

    msgSuffix = "Send";
    objSuffix = "Msg";
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
    \fn Flags::setMsgSuffix(QString newMsgSuffix)

    Sets the message suffix using \a newMsgSuffix.
    */
void Flags::setMsgSuffix(QString newMsgSuffix)
{
    msgSuffix = newMsgSuffix;
}

/*!
    \fn Flags::setObjSuffix(QString newObjSuffix)

    Sets the object suffix using \a newObjSuffix.
    */
void Flags::setObjSuffix(QString newObjSuffix)
{
    objSuffix = newObjSuffix;
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

/*!
  \fn Flags::messageSuffix() const

  Returns message suffix, which is appended to methods (ones that send the message) in generated code.
  */
QString Flags::messageSuffix() const
{
    return msgSuffix;
}

/*!
  \fn Flags::objectSuffix() const

  Returns the suffix, which is appended to object names (used in service header and source when in asynchronous).
  */
QString Flags::objectSuffix() const
{
    return objSuffix;
}
