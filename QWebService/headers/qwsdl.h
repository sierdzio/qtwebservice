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

#ifndef QWSDL_H
#define QWSDL_H

#include <QtCore/QXmlStreamReader>
#include <QtCore/qfile.h>
#include <QtCore/qmap.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qdatetime.h>
#include "QWebService_global.h"
#include "qwebservicemethod.h"

class QWsdlPrivate;

class QWEBSERVICESHARED_EXPORT QWsdl : public QObject
{
    Q_OBJECT

public:
    explicit QWsdl(QObject *parent = 0);
    QWsdl(const QString &wsdlFile, QObject *parent = 0);
    ~QWsdl();

    //QFile wsdl();
    QString wsdlFile() const;
    void setWsdlFile(const QString &wsdlFile); // == resetWsdl()
    void resetWsdl(const QString &newWsdl);

    QMap<QString, QWebServiceMethod *> *methods();
    QStringList methodNames() const;

    QString webServiceName() const;
    QString host() const;
    QUrl hostUrl() const;    
    QString targetNamespace() const;

    QString errorInfo() const;
    bool isErrorState() const;

signals:
    void errorEncountered(const QString &errMessage);

protected slots:
    void fileReplyFinished(QNetworkReply *rply);

protected:
    QWsdl(QWsdlPrivate &d, QObject *parent = 0);
    QWsdlPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QWsdl)

};

#endif // QWSDL_H
