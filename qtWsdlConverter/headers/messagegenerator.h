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

#ifndef MESSAGEGENERATOR_H
#define MESSAGEGENERATOR_H

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qmap.h>
#include <QtCore/qdir.h>
#include <QtCore/qtextstream.h>
#include <qwebservicemethod.h>
#include "flags.h"

class MessageGenerator : public QObject
{
    Q_OBJECT
public:
    enum FileType {
        Header        = 0,
        PrivateHeader = 1,
        Source        = 2
    };

    explicit MessageGenerator(QMap<QString, QWebServiceMethod *> *methods,
                              const QDir &workingDir, Flags *flgs, QObject *parent = 0);
    QString errorMessage();
    bool createMessages();

private:
    bool enterErrorState(const QString errMessage = QString());
    bool createSubclassedMessageHeader(QWebServiceMethod *msg);
    bool createSubclassedMessageSource(QWebServiceMethod *msg);
    bool createMessageHeader(QWebServiceMethod *msg);
    bool createMessageSource(QWebServiceMethod *msg);
    bool createMainCpp();

    void assignAllParameters(QWebServiceMethod *msg, QTextStream &out);
    QString assignAllParameters(QWebServiceMethod *msg);
    QString readFile(QString path);
    QString mergeHeaders(QString headerPath, QString privateHeaderPath);
    QString stripFile(QString fileData, FileType type = Header);

    QMap<QString, QWebServiceMethod *> *methods;
    QDir workingDir;
    Flags *flags;
    bool errorState;
    QString m_errorMessage;
};

#endif // MESSAGEGENERATOR_H
