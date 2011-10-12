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

#ifndef QWEBSERVICE_P_H
#define QWEBSERVICE_P_H

#include "qwebservice.h"
#include "qwebmethod.h"
#include "qwsdl.h"

class QWebServicePrivate
{

    Q_DECLARE_PUBLIC(QWebService)

public:
    QWebServicePrivate() {}
    QWebServicePrivate(QWebService *q) : q_ptr(q) {}
    QWebService *q_ptr;

    void init();
    bool enterErrorState(const QString &errMessage = QString());

    bool errorState;
    QString errorMessage;
    QString webServiceName;
    QUrl m_hostUrl;
    QWsdl *wsdl;
    // This is general, but should work for custom classes.
    QMap<QString, QWebServiceMethod *> *methods;
};

#endif // QWEBSERVICE_P_H
