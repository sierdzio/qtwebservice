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

#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qmap.h>
#include <QtCore/qdir.h>
#include <qwebservicemethod.h>
#include <qwsdl.h>
#include "flags.h"
#include "messagegenerator.h"

class CodeGenerator : public QObject
{
    Q_OBJECT
public:
    explicit CodeGenerator(QObject *parent = 0);
    static bool create(QWsdl *wsdl, const QDir &workingDir, Flags *flgs,
                       const QString &baseClassName = 0, QObject *parent = 0);
    bool isErrorState();

signals:
    void errorEncountered(const QString &errMessage);

public slots:

private:
    bool enterErrorState(const QString &errMessage = QString());
    void prepare();
    bool createMessages();
    bool createService();
    bool createServiceHeader();
    bool createServiceSource();
    bool createBuildSystemFile();
    bool createQMakeProject();
    bool createCMakeProject();
    bool createSconsProject();

    QMap<QString, QWebServiceMethod *> *methods;
    QDir workingDir;
    QWsdl *wsdl;
    Flags *flags;
    bool errorState;
    QString errorMessage;
    QString baseClassName;
};

#endif // CODEGENERATOR_H
