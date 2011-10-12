/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the QtDeveloperNetwork example.
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.txt included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#include <QtCore>
#if QT_VERSION < 0x050000
    #include <QtGui/QApplication>
    #include <QtGui/QMainWindow>
#else
    #include <QtWidgets/QApplication>
#endif
#include "qdnmain.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QdnMain w;
    w.show();

    return a.exec();
}
