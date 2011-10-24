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

#ifndef METHODGENERATOR_H
#define METHODGENERATOR_H

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qmap.h>
#include <QtCore/qdir.h>
#include <QtCore/qtextstream.h>
#include <qwebservicemethod.h>
#include "flags.h"
#include "templatelogic.h"

class MethodGenerator : public QObject
{
    Q_OBJECT
public:
    explicit MethodGenerator(QMap<QString, QWebServiceMethod *> *methods,
                              const QDir &workingDir, Flags *flgs, QObject *parent = 0);
    QString errorMessage();
    bool createMethods();

private:
    bool enterErrorState(const QString errMessage = QString());
    bool createSubclassedMethodHeader(QWebServiceMethod *mtd);
    bool createSubclassedMethodSource(QWebServiceMethod *mtd);
    bool createMethodHeader(QWebServiceMethod *mtd);
    bool createMethodSource(QWebServiceMethod *mtd);
    bool createMainCpp();

    void assignAllParameters(QWebServiceMethod *mtd, QTextStream &out);
    QString assignAllParameters(QWebServiceMethod *mtd);
    void addCustomCodeToConstructor(QString &sourceCode, QWebServiceMethod *mtd, int &beginIndex);

    QMap<QString, QWebServiceMethod *> *methods;
    QDir workingDir;
    Flags *flags;
    bool errorState;
    QString m_errorMessage;
    TemplateLogic logic;
};

#endif // METHODGENERATOR_H
