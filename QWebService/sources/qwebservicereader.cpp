/****************************************************************************
**
** Copyright (C) 2011 Tomasz Siekierda
** All rights reserved.
** Contact: Tomasz Siekierda (sierdzio@gmail.com)
**
** This file is part of the QWebService library, QtNetwork Module.
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

#include "../headers/qwebservicereader.h"

/*!
    \class QWebServiceReader
    \brief A QWebServiceAbstract derivative aimed specifically at communicatin
           with external web services.

    This class is in a very early stage of development and should be used
    with great caution, if at all.
  */

/*!
    Constructs QWebServiceAbstract with \a parent, defauling to 0.
  */
QWebServiceReader::QWebServiceReader(QObject *parent) :
    QWebService(parent)
{
}

/*!
    Constructs QWebServiceAbstract with \a wsdl and \a parent
    (which defaults to 0).
  */
QWebServiceReader::QWebServiceReader(QWsdl *wsdl, QObject *parent) :
    QWebService(wsdl, parent)
{
}

/*!
    Constructs QWebServiceAbstract with \a parent and \a hostname
    (which is used to create a wsdl object).
  */
QWebServiceReader::QWebServiceReader(QString hostname, QObject *parent) :
    QWebService(new QWsdl(hostname, parent), parent)
{

}

/*!
    Pure virtual method, can be used to handle methods. Probably a bad idea,
    might be dropped in the future.
    Accepts parameters in a QVariant \a param.
  */
void QWebServiceReader::methodHandler(QVariant param)
{
    Q_UNUSED(param)
}
