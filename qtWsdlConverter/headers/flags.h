#ifndef FLAGS_H
#define FLAGS_H

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>

class Flags : public QObject
{
    Q_OBJECT
    Q_FLAGS(Options)
    Q_ENUMS(HttpMethod)

public:
    enum Option
    {
        // Mode
        fullMode                = 0x000001,
        debugMode               = 0x000002,
        compactMode             = 0x000004,
        subclass                = 0x000008,
        // Synchronousness
        synchronous             = 0x000010,
        asynchronous            = 0x000020,
        // Structure
        standardStructure       = 0x000100,
        noMessagesStructure     = 0x000200,
        allInOneDirStructure    = 0x000400,
        // Build system
        qmake                   = 0x001000,
        cmake                   = 0x002000,
        scons                   = 0x004000,
        noBuildSystem           = 0x008000,
        // Protocol
        http                    = 0x010000,
        soap10                  = 0x020000,
        soap12                  = 0x040000,
        soap                    = 0x060000,
        json                    = 0x080000,
        xml                     = 0x100000,
        rest                    = 0x200000
    };
    Q_DECLARE_FLAGS(Options, Option)

    enum HttpMethod
    {
        POST    = 0x1,
        GET     = 0x2,
        PUT     = 0x4,
        DELETE  = 0x8
    };

    Flags(Options options = Options(fullMode | synchronous | standardStructure | qmake | soap12),
          HttpMethod method = POST,
          bool force = false,
          QObject *parent = 0);

    void resetFlags();
    void resetFlags(Options whatToReset);
    void setFlags(Options options);
    void setHttpMethod(HttpMethod newMethod);
    bool setHttpMethod(QString newMethod);
    void setForced(bool forced);
    void setMsgSuffix(QString newMsgSuffix);
    void setObjSuffix(QString newObjSuffix);
    Options flags() const;
    HttpMethod httpMethod() const;
    bool forced() const;
    bool isForced() const;
    QString messageSuffix() const;
    QString objectSuffix() const;
    // Convenience getters:
    QString httpMethodString() const;
    QString protocolString(bool includeRest = false) const;

    Flags & operator=(const Flags &flag);

private:
    Options options;
    HttpMethod method;
    bool force;
    QString msgSuffix;
    QString objSuffix;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Flags::Options)

#endif // FLAGS_H
