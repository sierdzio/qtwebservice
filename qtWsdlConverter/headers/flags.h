/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the qtWsdlConverter tool.
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.txt included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

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

    Options flags() const;
    void setFlags(Options options);
    void resetFlags();
    void resetFlags(Options whatToReset);
    QString protocolString(bool includeRest = false) const;

    HttpMethod httpMethod() const;
    QString httpMethodString() const;
    void setHttpMethod(HttpMethod newMethod);
    bool setHttpMethod(const QString &newMethod);

    bool forced() const;
    bool isForced() const;
    void setForced(bool forced);

    QString methodSuffix() const;
    void setMethodSuffix(const QString &newMsgSuffix);

    QString objectSuffix() const;
    void setObjectSuffix(const QString &newObjSuffix);

    QString endLine() const;
    void setEndLine(const QString &newEndLine);

    QString tab() const;
    void setTab(int numberOfSpaces);

    Flags & operator=(const Flags &flag);

private:
    Options options;
    HttpMethod method;
    bool force;
    QString msgSuffix;
    QString objSuffix;
    QString endLn;
    QString tabulation;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Flags::Options)

#endif // FLAGS_H
