/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the QWebService library.
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.txt included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#ifndef QWSDL_P_H
#define QWSDL_P_H

#include <QtCore/QXmlStreamReader>
#include <QtCore/qfile.h>
#include <QtCore/qmap.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qdatetime.h>
#include "qwebservicemethod.h"
#include "qwsdl.h"

class QWsdlPrivate
{
    Q_DECLARE_PUBLIC(QWsdl)

public:
    QWsdlPrivate() {}
    QWsdlPrivate(QWsdl *q) : q_ptr(q) {}
    QWsdl *q_ptr;

    void init();
//    bool parse();
//    void prepareFile();
    void prepareMethods();
    void readDefinitions();
    void readTypes();
    void readTypeSchemaElement();
    void readPorts();
    void readMessages();
    void readBindings();
    void readService();
    void readDocumentation();
    QString convertReplyToUtf(const QString &textToConvert);
    bool enterErrorState(const QString &errMessage = QString());

    bool errorState;
    bool replyReceived;
    QUrl m_hostUrl;
    QString errorMessage;
    QString m_wsdlFilePath;
    QString m_webServiceName;
    QString m_targetNamespace;
    QXmlStreamReader xmlReader;

    QStringList *workMethodList;
    // Param if one, QList if many.
    QMap<int, QMap<QString, QVariant> > *workMethodParameters;
    QMap<QString, QWebMethod *> *methodsMap;
};

#endif // QWSDL_P_H
