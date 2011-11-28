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

    Q_PROPERTY(QString webServiceName READ webServiceName NOTIFY wsdlFileChanged)
    Q_PROPERTY(QString host READ host NOTIFY wsdlFileChanged)
    Q_PROPERTY(QUrl hostUrl READ hostUrl NOTIFY wsdlFileChanged)
    Q_PROPERTY(QString targetNamespace READ targetNamespace NOTIFY wsdlFileChanged)
    Q_PROPERTY(QString wsdlFile READ wsdlFile WRITE setWsdlFile NOTIFY wsdlFileChanged)

public:
    explicit QWsdl(QObject *parent = 0);
    QWsdl(const QString &wsdlFile, QObject *parent = 0);
    ~QWsdl();

    //QFile wsdl();
    QString wsdlFile() const;
    void setWsdlFile(const QString &wsdlFile); // == resetWsdl()
    void resetWsdl(const QString &newWsdl);

    QMap<QString, QWebMethod *> *methods();
    QStringList methodNames() const;

    QString webServiceName() const;
    QString host() const;
    QUrl hostUrl() const;    
    QString targetNamespace() const;

    QString errorInfo() const;
    bool isErrorState() const;

    bool parse();

signals:
    void errorEncountered(const QString &errMessage);

    // For QObject properties:
    void wsdlFileChanged();

protected slots:
    void fileReplyFinished(QNetworkReply *rply);

protected:
    QWsdl(QWsdlPrivate &d, QObject *parent = 0);
    QWsdlPrivate *d_ptr;

private:
    void prepareFile(); // temp - problems in PIMPL

    Q_DECLARE_PRIVATE(QWsdl)

};

#endif // QWSDL_H
