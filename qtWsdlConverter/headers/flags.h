#ifndef FLAGS_H
#define FLAGS_H

#include <QWebService>

/**
  Holds all converter's switches in one place.
  WARNING: all flag variables are PUBLIC, so be careful not to assign value instead of comparing!
  */
class Flags
{
    Q_ENUMS(Option)
    Q_FLAGS(Options)

public:
    // Experimental flag redoing:
    enum Option
    {
        // Mode
        fullMode                = 0x00001,
        debugMode               = 0x00002,
        compactMode             = 0x00004,
        // Synchronousness
        synchronous             = 0x00010,
        asynchronous            = 0x00020,
        // Structure
        standardStructure       = 0x00100,
        noMessagesStructure     = 0x00200,
        allInOneDirStructure    = 0x00400,
        // Build system
        qmake                   = 0x01000,
        cmake                   = 0x02000,
        scons                   = 0x04000,
        noBuildSystem           = 0x08000,
        // Protocol
        http                    = 0x10000,
        soap10                  = 0x20000,
        soap12                  = 0x40000,
        soap                    = 0x60000,
        json                    = 0x80000
    };

    Q_DECLARE_FLAGS(Options, Option)

    Flags(Options options = Options(fullMode | synchronous | standardStructure | qmake | soap12), bool force = false);

    void resetFlags();
    void setFlags(Options options);
    void setForced(bool forced);
    Options flags() const;
    bool forced() const;
    bool isForced() const;

private:
    Options options;
    bool force;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Flags::Options)

#endif // FLAGS_H
