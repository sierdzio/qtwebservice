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

#ifndef QWEBSERVICEMETHOD_P_H
#define QWEBSERVICEMETHOD_P_H

#include "qwebmethod_p.h"
#include "qwebservicemethod.h"

class QWebServiceMethodPrivate : public QWebMethodPrivate
{
    Q_DECLARE_PUBLIC(QWebServiceMethod)

public:
    QWebServiceMethodPrivate() {}
};

#endif // QWEBSERVICEMETHOD_P_H
