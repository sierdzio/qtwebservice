/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the qtWsdlConverter tool.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
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

    explicit WsdlConverter(QStringList appArguments, QObject *parent = 0);
    ~WsdlConverter();

    void setFlags(Flags flgs);
    bool isErrorState();
    QString errorInfo();
    void resetError();
    void convert();
    QString webServiceName();

signals:
    void errorEncountered(QString errMessage);

public slots:

private:
    bool parseArguments(QStringList arguments);
    void displayHelp();
    void displayIntro();
    void displayOutro();

    bool enterErrorState(QString errMessage = "");
    bool removeDir(QString path);

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
