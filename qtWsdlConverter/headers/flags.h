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
        FullMode                = 0x000001,
        DebugMode               = 0x000002,
        CompactMode             = 0x000004,
        Subclass                = 0x000008,
        // Synchronousness
        Synchronous             = 0x000010,
        Asynchronous            = 0x000020,
        // Structure
        StandardStructure       = 0x000100,
        NoMessagesStructure     = 0x000200,
        AllInOneDirStructure    = 0x000400,
        // Build system
        Qmake                   = 0x001000,
        Cmake                   = 0x002000,
        Scons                   = 0x004000,
        NoBuildSystem           = 0x008000,
        // Protocol
        Http                    = 0x010000,
        Soap10                  = 0x020000,
        Soap12                  = 0x040000,
        Soap                    = 0x060000,
        Json                    = 0x080000,
        Xml                     = 0x100000,
        Rest                    = 0x200000
    };
    Q_DECLARE_FLAGS(Options, Option)

    enum HttpMethod
    {
        Post    = 0x1,
        Get     = 0x2,
        Put     = 0x4,
        Delete  = 0x8
    };

    Flags(Options options = Options(FullMode | Synchronous | StandardStructure | Qmake | Soap12),
          HttpMethod method = Post,
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
