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

class QWebServicePrivate;

class QWEBSERVICESHARED_EXPORT QWebService : public QObject
{
    Q_OBJECT

public:
    QWebService(QObject *parent = 0);
    QWebService(QWsdl *wsdl, QObject *parent = 0);
    QWebService(const QString &host, QObject *parent = 0);
    ~QWebService();

    QString name() const;
    QStringList methodNames() const;
    QStringList methodParameters(const QString &methodName) const;
    QStringList methodReturnValue(const QString &methodName) const;
    QMap<QString, QVariant> parameterNamesTypes(const QString &methodName) const;
    QMap<QString, QVariant> returnValueNameType(const QString &methodName) const;
    QMap<QString, QWebServiceMethod *> *methods();
    QWebServiceMethod *method(const QString &methodName);

    void addMethod(QWebServiceMethod *newMethod);
    void addMethod(const QString &methodName, QWebServiceMethod *newMethod);
    void removeMethod(const QString &methodName);
    void setHost(const QString &host);
    void setHost(const QUrl &hostUrl);
    void setName(const QString &newWebServiceName = QString());
    void setWsdl(QWsdl *newWsdl);
    void resetWsdl(QWsdl *newWsdl = 0);
    QUrl hostUrl() const;
    QString host() const;
    bool isErrorState();
    QString errorInfo() const;
//    QString getWsdl();

signals:
    void errorEncountered(const QString &errMessage);

protected:
    QWebService(QWebServicePrivate &d, QObject *parent = 0);
    QWebServicePrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QWebService)
};

#endif // QWEBSERVICE_H
