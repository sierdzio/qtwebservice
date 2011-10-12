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

#ifndef WSDLCONVERTER_H
#define WSDLCONVERTER_H

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qmap.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qvariant.h>
#include <QtCore/qcoreapplication.h>
#include <QtDebug>
#include <qwsdl.h>
#include "flags.h"
#include "codegenerator.h"

class WsdlConverter : public QObject
{
    Q_OBJECT
    Q_ENUMS(ArgumentDescriptionFlag)
    Q_FLAGS(ArgumentDescription)

public:
    enum ArgumentDescriptionFlag
    {
        AppName     = 0x01,
        Path        = 0x02,
        Dir         = 0x04,
        ClassName   = 0x08,
        FlagHelp    = 0x0F
    };
    Q_DECLARE_FLAGS(ArgumentDescription, ArgumentDescriptionFlag)

    explicit WsdlConverter(const QStringList &appArguments, QObject *parent = 0);
    ~WsdlConverter();

    void setFlags(Flags flgs);
    bool isErrorState();
    QString errorInfo();
    void resetError();
    void convert();
    QString webServiceName();

signals:
    void errorEncountered(const QString &errMessage);

public slots:

private:
    bool parseArguments(const QStringList &arguments);
    void displayHelp();
    void displayIntro();
    void displayOutro();

    bool enterErrorState(const QString &errMessage = QString());
    bool removeDir(const QString &path);

    bool errorState;
    QString errorMessage;
    QString baseClassName;
    QString outputDir;
    QWsdl *wsdl;
    QMap<int, QVariant> *argList;
    Flags *flags;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(WsdlConverter::ArgumentDescription)

#endif // WSDLCONVERTER_H
