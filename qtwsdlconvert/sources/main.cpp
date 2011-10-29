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

#include <QtCore/qcoreapplication.h>
#include "../headers/wsdlconverter.h"

/**
  * qtwsdlconverter's main routine.
  */
int main(int argc, char *argv[])
{    
    QCoreApplication a(argc, argv);

    WsdlConverter converter(a.arguments());
    if (!converter.isErrorState()) {
        converter.convert();
    }

    if (converter.isErrorState())
        printf(converter.errorInfo().toLocal8Bit());

    return 0;//    a.exec();
}
