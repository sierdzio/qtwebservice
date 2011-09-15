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

#ifndef QWEBSERVICE_H
#define QWEBSERVICE_H

#include <QtCore/qurl.h>
#include "QWebService_global.h"
#include "qwebmethod.h"
#include "qwsdl.h"


class QWEBSERVICESHARED_EXPORT QWebService : public QObject
{
    Q_OBJECT

public:
    QWebService(QObject *parent = 0);
    QWebService(QWsdl *wsdl, QObject *parent = 0);
    QWebService(QString host, QObject *parent = 0);
    ~QWebService();

    QStringList methodNames() const;
    QStringList methodParameters(QString methodName) const;
    QStringList methodReturnValue(QString methodName) const;
    QMap<QString, QVariant> parameterNamesTypes(QString methodName) const;
    QMap<QString, QVariant> returnValueNameType(QString methodName) const;

    void addMethod(QWebServiceMethod *newMethod);
    void addMethod(QString methodName, QWebServiceMethod *newMethod);
    void removeMethod(QString methodName);
    void setHost(QString host);
    void setHost(QUrl hostUrl);
    void setWsdl(QWsdl *newWsdl);
    void resetWsdl(QWsdl *newWsdl = 0);
    QUrl hostUrl() const;
    QString host() const;
    bool isErrorState();
    QString errorInfo() const;
//    QString getWsdl();

signals:
    void errorEncountered(QString errMessage);

protected:
    void init();
    bool enterErrorState(QString errMessage = "");

    bool errorState;
    QString errorMessage;
    QUrl m_hostUrl;
    QWsdl *wsdl;
    // This is general, but should work for custom classes.
    QMap<QString, QWebServiceMethod *> *methods;
};

#endif // QWEBSERVICE_H
